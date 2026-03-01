#pragma once

#include "EarthCore/Core/PotatoEarthBase.h"
#include "Tile.h"
#include "EarthCore/Render/Texture.h"
#include <future>
#include <queue>
#include <mutex>

namespace PTEarth {

    enum class ImageryProviderType {
        OpenStreetMap,
        BingMaps,
        Custom
    };

    struct ImageryTileRequest {
        TileID ID;
        std::string URL;
        std::promise<PT_Ref<Texture>> Promise;
    };

    class ImageryLayer {
    public:
        ImageryLayer(const std::string& name, ImageryProviderType type);
        ~ImageryLayer();

        void Initialize();
        void Shutdown();

        // Get texture for a tile
        PT_Ref<Texture> GetTileTexture(const TileID& id);
        bool IsTileAvailable(const TileID& id) const;

        // Async loading
        void ProcessLoadingQueue();

        // Configuration
        void SetMaxConcurrentRequests(size_t max) { m_MaxConcurrentRequests = max; }

        const std::string& GetName() const { return m_Name; }

    private:
        std::string BuildURL(const TileID& id) const;
        void DownloadTile(const TileID& id);
        std::string GetCachePath(const TileID& id) const;

    private:
        std::string m_Name;
        ImageryProviderType m_ProviderType;

        // Tile cache (in memory)
        std::unordered_map<TileID, PT_Ref<Texture>, TileIDHash> m_TileCache;
        std::mutex m_CacheMutex;

        // Loading queue
        std::queue<TileID> m_LoadQueue;
        std::mutex m_QueueMutex;

        // Active downloads
        std::vector<std::future<void>> m_ActiveDownloads;
        size_t m_MaxConcurrentRequests = 4;
    };

}
