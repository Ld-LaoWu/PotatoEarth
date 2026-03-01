#pragma once

#include "EarthCore/Core/PotatoEarthBase.h"
#include "Terrain.h"
#include "ImageryLayer.h"
#include "Entity.h"
#include <string>
#include <vector>
#include <queue>
#include <future>

namespace PTEarth {

    // Data source interface for streaming data
    class DataSource {
    public:
        virtual ~DataSource() = default;
        
        // Connection
        virtual bool Connect(const std::string& url) = 0;
        virtual void Disconnect() = 0;
        virtual bool IsConnected() const = 0;
        
        // Request data
        virtual std::future<std::vector<uint8_t>> Request(const std::string& endpoint) = 0;
        
        // Settings
        void SetTimeout(uint32_t seconds) { m_Timeout = seconds; }
        void SetMaxRetries(uint32_t retries) { m_MaxRetries = retries; }
        
    protected:
        uint32_t m_Timeout = 30;
        uint32_t m_MaxRetries = 3;
    };

    // HTTP/HTTPS data source
    class HTTPDataSource : public DataSource {
    public:
        bool Connect(const std::string& url) override;
        void Disconnect() override;
        bool IsConnected() const override { return m_Connected; }
        std::future<std::vector<uint8_t>> Request(const std::string& endpoint) override;
        
    private:
        std::string m_BaseURL;
        bool m_Connected = false;
    };

    // WebSocket for real-time data
    class WebSocketDataSource : public DataSource {
    public:
        bool Connect(const std::string& url) override;
        void Disconnect() override;
        bool IsConnected() const override { return m_Connected; }
        std::future<std::vector<uint8_t>> Request(const std::string& endpoint) override;
        
        // Subscribe to real-time updates
        void Subscribe(const std::string& channel, std::function<void(const std::vector<uint8_t>&)> callback);
        void Unsubscribe(const std::string& channel);
        
    private:
        std::string m_URL;
        bool m_Connected = false;
        std::map<std::string, std::function<void(const std::vector<uint8_t>&)>> m_Subscriptions;
    };

    // Streaming terrain provider
    class StreamingTerrainProvider : public TerrainProvider {
    public:
        StreamingTerrainProvider(PT_Ref<DataSource> dataSource);
        
        TerrainTile* GetTile(const TileID& id) override;
        HeightSample SampleHeight(const Cartographic& position) override;
        bool IsTileAvailable(const TileID& id) override;
        
        void Update();  // Process pending requests
        
    private:
        PT_Ref<DataSource> m_DataSource;
        std::queue<TileID> m_RequestQueue;
        std::vector<std::future<TerrainTile*>> m_PendingRequests;
    };

    // Streaming imagery provider
    class StreamingImageryProvider {
    public:
        StreamingImageryProvider(PT_Ref<DataSource> dataSource);
        
        PT_Ref<Texture> GetTileTexture(const TileID& id);
        bool IsTileAvailable(const TileID& id) const;
        
        void Update();
        
    private:
        PT_Ref<DataSource> m_DataSource;
        std::queue<TileID> m_RequestQueue;
        std::vector<std::future<PT_Ref<Texture>>> m_PendingRequests;
    };

    // Real-time entity streaming (for tracking, live data)
    class EntityStreamer {
    public:
        EntityStreamer(PT_Ref<DataSource> dataSource);
        
        void StartStreaming();
        void StopStreaming();
        
        // Callback for new entity data
        void SetEntityUpdateCallback(std::function<void(const std::string& entityId, const Cartographic& position)> callback);
        
    private:
        PT_Ref<DataSource> m_DataSource;
        std::function<void(const std::string&, const Cartographic&)> m_UpdateCallback;
        bool m_Streaming = false;
    };

    // Network manager
    class NetworkManager {
    public:
        void Initialize();
        void Shutdown();
        
        // Create data sources
        PT_Ref<DataSource> CreateHTTPSource(const std::string& baseURL);
        PT_Ref<DataSource> CreateWebSocketSource(const std::string& url);
        
        // Update all streaming sources
        void Update();
        
    private:
        std::vector<PT_Ref<DataSource>> m_DataSources;
    };

}
