#pragma once

#include "EarthCore/Core/PotatoEarthBase.h"
#include "EarthCore/Scene/Tile.h"
#include "EarthCore/Render/Texture.h"
#include <string>
#include <vector>
#include <queue>
#include <future>

namespace PTEarth {

    // Imagery provider types
    enum class ImageryProviderType {
        BingMaps,
        OpenStreetMap,
        Mapbox,
        GoogleEarth,
        Sentinel,
        SingleTile,
        TMS,
        UrlTemplate
    };

    // Imagery layer
    class ImageryLayer {
    public:
        ImageryLayer(const std::string& name, ImageryProviderType type);
        ~ImageryLayer();

        void Initialize();
        void Shutdown();

        // Get texture for tile
        PT_Ref<Texture> GetTileTexture(const TileID& id);
        bool IsTileAvailable(const TileID& id) const;

        // Async loading
        void Update();

        // Configuration
        void SetAlpha(float alpha) { m_Alpha = alpha; }
        void SetBrightness(float brightness) { m_Brightness = brightness; }
        void SetContrast(float contrast) { m_Contrast = contrast; }
        void SetSaturation(float saturation) { m_Saturation = saturation; }
        void SetGamma(float gamma) { m_Gamma = gamma; }
        void SetVisible(bool visible) { m_Visible = visible; }

        float GetAlpha() const { return m_Alpha; }
        float GetBrightness() const { return m_Brightness; }
        float GetContrast() const { return m_Contrast; }
        float GetSaturation() const { return m_Saturation; }
        float GetGamma() const { return m_Gamma; }
        bool IsVisible() const { return m_Visible; }

        const std::string& GetName() const { return m_Name; }

    private:
        std::string BuildURL(const TileID& id) const;
        void LoadTile(const TileID& id);
        std::string GetCachePath(const TileID& id) const;

    private:
        std::string m_Name;
        ImageryProviderType m_Type;

        // Style properties
        float m_Alpha = 1.0f;
        float m_Brightness = 1.0f;
        float m_Contrast = 1.0f;
        float m_Saturation = 1.0f;
        float m_Gamma = 1.0f;
        bool m_Visible = true;

        // Tile cache
        std::unordered_map<TileID, PT_Ref<Texture>, TileIDHash> m_TileCache;
        std::mutex m_CacheMutex;

        // Loading queue
        std::queue<TileID> m_LoadQueue;
        std::mutex m_QueueMutex;

        // Async downloads
        std::vector<std::future<void>> m_PendingDownloads;
        size_t m_MaxConcurrentDownloads = 4;
    };

    // Imagery layer collection
    class ImageryLayerCollection {
    public:
        void AddLayer(PT_Ref<ImageryLayer> layer);
        void RemoveLayer(const std::string& name);
        void MoveLayer(const std::string& name, int newIndex);
        
        PT_Ref<ImageryLayer> GetLayer(const std::string& name);
        PT_Ref<ImageryLayer> GetLayer(int index);
        int GetLayerCount() const { return (int)m_Layers.size(); }
        
        void UpdateAll();
        
        // Get combined texture for tile (blend all layers)
        // PT_Ref<Texture> GetCombinedTexture(const TileID& id);

    private:
        std::vector<PT_Ref<ImageryLayer>> m_Layers;
    };

}
