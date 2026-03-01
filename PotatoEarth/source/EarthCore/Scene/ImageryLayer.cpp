#include "potatopch.h"
#include "ImageryLayer.h"

#include "EarthCore/Core/Log.h"
#include <filesystem>
#include <fstream>
#include <url/curl.h>

namespace PTEarth {

    // curl write callback
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

    ImageryLayer::ImageryLayer(const std::string& name, ImageryProviderType type)
        : m_Name(name), m_ProviderType(type) {
    }

    ImageryLayer::~ImageryLayer() {
        Shutdown();
    }

    void ImageryLayer::Initialize() {
        // Create cache directory
        std::string cacheDir = "cache/imagery/" + m_Name;
        std::filesystem::create_directories(cacheDir);

        PT_EARTH_INFO("ImageryLayer '{0}' initialized", m_Name);
    }

    void ImageryLayer::Shutdown() {
        // Wait for all downloads to complete
        for (auto& future : m_ActiveDownloads) {
            if (future.valid()) {
                future.wait();
            }
        }
    }

    PT_Ref<Texture> ImageryLayer::GetTileTexture(const TileID& id) {
        // Check cache first
        {
            std::lock_guard<std::mutex> lock(m_CacheMutex);
            auto it = m_TileCache.find(id);
            if (it != m_TileCache.end()) {
                return it->second;
            }
        }

        // Try to load from disk cache
        std::string cachePath = GetCachePath(id);
        if (std::filesystem::exists(cachePath)) {
            auto texture = Texture::Create(cachePath);
            if (texture->IsLoaded()) {
                std::lock_guard<std::mutex> lock(m_CacheMutex);
                m_TileCache[id] = texture;
                return texture;
            }
        }

        // Queue for download
        {
            std::lock_guard<std::mutex> lock(m_QueueMutex);
            m_LoadQueue.push(id);
        }

        return nullptr;
    }

    bool ImageryLayer::IsTileAvailable(const TileID& id) const {
        // Check if in memory cache
        {
            std::lock_guard<std::mutex> lock(m_CacheMutex);
            if (m_TileCache.find(id) != m_TileCache.end()) {
                return true;
            }
        }

        // Check if in disk cache
        std::string cachePath = GetCachePath(id);
        return std::filesystem::exists(cachePath);
    }

    void ImageryLayer::ProcessLoadingQueue() {
        // Process download queue
        while (!m_LoadQueue.empty() && m_ActiveDownloads.size() < m_MaxConcurrentRequests) {
            TileID id;
            {
                std::lock_guard<std::mutex> lock(m_QueueMutex);
                id = m_LoadQueue.front();
                m_LoadQueue.pop();
            }

            // Start async download
            auto future = std::async(std::launch::async, [this, id]() {
                DownloadTile(id);
            });
            m_ActiveDownloads.push_back(std::move(future));
        }

        // Clean up completed downloads
        m_ActiveDownloads.erase(
            std::remove_if(m_ActiveDownloads.begin(), m_ActiveDownloads.end(),
                [](const std::future<void>& f) {
                    return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
                }),
            m_ActiveDownloads.end()
        );
    }

    std::string ImageryLayer::BuildURL(const TileID& id) const {
        switch (m_ProviderType) {
            case ImageryProviderType::OpenStreetMap:
                // OSM tile URL format
                return "https://tile.openstreetmap.org/" + 
                       std::to_string(id.Level) + "/" + 
                       std::to_string(id.X) + "/" + 
                       std::to_string(id.Y) + ".png";
            
            case ImageryProviderType::BingMaps:
                // Bing Maps would require API key
                return "";
            
            default:
                return "";
        }
    }

    void ImageryLayer::DownloadTile(const TileID& id) {
        std::string url = BuildURL(id);
        if (url.empty()) return;

        CURL* curl = curl_easy_init();
        if (!curl) return;

        std::string responseData;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "PotatoEarth/1.0");

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            PT_EARTH_ERROR("Failed to download tile {0}/{1}/{2}: {3}", 
                id.Level, id.X, id.Y, curl_easy_strerror(res));
            return;
        }

        // Save to cache
        std::string cachePath = GetCachePath(id);
        std::ofstream file(cachePath, std::ios::binary);
        if (file.is_open()) {
            file.write(responseData.data(), responseData.size());
            file.close();

            // Load texture
            auto texture = Texture::Create(cachePath);
            if (texture->IsLoaded()) {
                std::lock_guard<std::mutex> lock(m_CacheMutex);
                m_TileCache[id] = texture;
            }
        }
    }

    std::string ImageryLayer::GetCachePath(const TileID& id) const {
        return "cache/imagery/" + m_Name + "/" + 
               std::to_string(id.Level) + "/" + 
               std::to_string(id.X) + "/" + 
               std::to_string(id.Y) + ".png";
    }

}
