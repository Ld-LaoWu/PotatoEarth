#pragma once

#include "EarthCore/Core/PotatoEarthBase.h"
#include "Tile.h"
#include <vector>
#include <future>

namespace PTEarth {

    // Terrain data formats
    enum class TerrainFormat {
        HeightmapPNG,    // PNG grayscale heightmap
        QuantizedMesh,   // Cesium quantized-mesh format
        TerrainDB        // Database format
    };

    // Height sample result
    struct HeightSample {
        float Height;           // Height in meters
        bool IsValid;           // Whether sample is valid
        float Accuracy;         // Estimated accuracy
    };

    // Terrain tile data
    class TerrainTile {
    public:
        TileID ID;
        std::vector<float> Heightmap;  // Height values
        uint32_t Resolution;            // Grid resolution (e.g., 64x64)
        float MinHeight;                // Minimum height in tile
        float MaxHeight;                // Maximum height in tile
        
        // Generate mesh with height
        void GenerateMesh();
        PT_Ref<VertexArray> GetMesh() { return m_Mesh; }
        
        // Sample height at geographic position
        float SampleHeight(double lon, double lat) const;
        
    private:
        PT_Ref<VertexArray> m_Mesh;
    };

    // Terrain provider interface
    class TerrainProvider {
    public:
        virtual ~TerrainProvider() = default;
        
        // Get terrain tile
        virtual TerrainTile* GetTile(const TileID& id) = 0;
        
        // Sample height at position
        virtual HeightSample SampleHeight(const Cartographic& position) = 0;
        
        // Check if terrain is available
        virtual bool IsReady() const { return true; }
        
        // Get terrain availability
        virtual bool IsTileAvailable(const TileID& id) = 0;
        
        // Maximum terrain level
        virtual int GetMaxLevel() const { return 18; }
    };

    // Ellipsoid terrain (no actual terrain, just WGS84)
    class EllipsoidTerrainProvider : public TerrainProvider {
    public:
        TerrainTile* GetTile(const TileID& id) override;
        HeightSample SampleHeight(const Cartographic& position) override;
        bool IsTileAvailable(const TileID& id) override { return true; }
    };

    // Online terrain provider (Cesium ion, Mapbox, etc.)
    class OnlineTerrainProvider : public TerrainProvider {
    public:
        OnlineTerrainProvider(const std::string& url);
        
        TerrainTile* GetTile(const TileID& id) override;
        HeightSample SampleHeight(const Cartographic& position) override;
        bool IsTileAvailable(const TileID& id) override;
        
        // Async loading
        void RequestTile(const TileID& id);
        void ProcessRequests();
        
    private:
        std::string m_BaseURL;
        std::unordered_map<TileID, TerrainTile*, TileIDHash> m_Cache;
        std::queue<TileID> m_LoadQueue;
    };

    // Terrain system manager
    class TerrainSystem {
    public:
        TerrainSystem();
        ~TerrainSystem();
        
        void Initialize();
        void Shutdown();
        
        // Set terrain provider
        void SetProvider(PT_Ref<TerrainProvider> provider) { m_Provider = provider; }
        PT_Ref<TerrainProvider> GetProvider() const { return m_Provider; }
        
        // Sample height
        float GetHeight(double longitude, double latitude);
        
        // Get tile with terrain
        TerrainTile* GetTerrainTile(const TileID& id);
        
        // Update (async loading)
        void Update();
        
    private:
        PT_Ref<TerrainProvider> m_Provider;
        std::unordered_map<TileID, TerrainTile*, TileIDHash> m_TileCache;
    };

}


