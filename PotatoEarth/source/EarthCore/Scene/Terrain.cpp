#include "potatopch.h"
#include "Terrain.h"

#include "EarthCore/Core/Geospatial/Ellipsoid.h"
#include "EarthCore/Render/Buffer.h"
#include <glad/glad.h>
#include <math.h>

namespace PTEarth {

    // EllipsoidTerrainProvider
    TerrainTile* EllipsoidTerrainProvider::GetTile(const TileID& id) {
        auto* tile = new TerrainTile();
        tile->ID = id;
        tile->Resolution = 2;
        tile->Heightmap = {0.0f, 0.0f, 0.0f, 0.0f};  // Flat at 0 height
        tile->MinHeight = 0.0f;
        tile->MaxHeight = 0.0f;
        return tile;
    }

    HeightSample EllipsoidTerrainProvider::SampleHeight(const Cartographic& position) {
        return {0.0f, true, 1.0f};  // Ellipsoid surface is at 0 height
    }

    // TerrainTile
    void TerrainTile::GenerateMesh() {
        if (Heightmap.empty()) return;
        
        std::vector<float> vertices;
        std::vector<uint32_t> indices;
        
        // Calculate bounds
        int n = 1 << ID.Level;
        double lonWest = (ID.X / (double)n) * 2.0 * PTMath::OnePi - PTMath::OnePi;
        double lonEast = ((ID.X + 1) / (double)n) * 2.0 * PTMath::OnePi - PTMath::OnePi;
        
        double mercatorY_n = PTMath::OnePi * (1 - 2 * ID.Y / (double)n);
        double mercatorY_s = PTMath::OnePi * (1 - 2 * (ID.Y + 1) / (double)n);
        double latNorth = atan(sinh(mercatorY_n));
        double latSouth = atan(sinh(mercatorY_s));
        
        uint32_t segments = Resolution - 1;
        
        for (uint32_t y = 0; y <= segments; ++y) {
            double latFrac = y / (double)segments;
            double lat = latSouth * (1.0 - latFrac) + latNorth * latFrac;
            
            for (uint32_t x = 0; x <= segments; ++x) {
                double lonFrac = x / (double)segments;
                double lon = lonWest * (1.0 - lonFrac) + lonEast * lonFrac;
                
                // Get height from heightmap
                uint32_t hx = x * (Resolution - 1) / segments;
                uint32_t hy = y * (Resolution - 1) / segments;
                float height = Heightmap[hy * Resolution + hx];
                
                // Position on ellipsoid + height
                Cartographic carto(lon, lat, height);
                glm::dvec3 pos = Ellipsoid::WGS84.cartographicToCartesian(carto);
                glm::dvec3 normal = Ellipsoid::WGS84.geodeticSurfaceNormal(carto);
                
                vertices.push_back((float)pos.x / 1000000.0f);
                vertices.push_back((float)pos.y / 1000000.0f);
                vertices.push_back((float)pos.z / 1000000.0f);
                vertices.push_back((float)normal.x);
                vertices.push_back((float)normal.y);
                vertices.push_back((float)normal.z);
                vertices.push_back((float)lonFrac);
                vertices.push_back((float)latFrac);
            }
        }
        
        // Generate indices
        for (uint32_t y = 0; y < segments; ++y) {
            for (uint32_t x = 0; x < segments; ++x) {
                uint32_t current = y * (segments + 1) + x;
                uint32_t next = current + segments + 1;
                
                indices.push_back(current);
                indices.push_back(next);
                indices.push_back(current + 1);
                
                indices.push_back(current + 1);
                indices.push_back(next);
                indices.push_back(next + 1);
            }
        }
        
        m_Mesh = VertexArray::Create();
        
        PT_Ref<VertexBuffer> vb = VertexBuffer::Create(vertices.data(), vertices.size() * sizeof(float));
        BufferLayout layout = {
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float3, "a_Normal" },
            { ShaderDataType::Float2, "a_TexCoord" }
        };
        vb->SetLayout(layout);
        
        PT_Ref<IndexBuffer> ib = IndexBuffer::Create(indices.data(), indices.size());
        
        m_Mesh->AddVertexBuffer(vb);
        m_Mesh->SetIndexBuffer(ib);
    }

    float TerrainTile::SampleHeight(double lon, double lat) const {
        // Bilinear interpolation
        // This is a simplified version
        if (Heightmap.empty()) return 0.0f;
        return Heightmap[0];  // Return first value as approximation
    }

    // TerrainSystem
    TerrainSystem::TerrainSystem() {
        // Default to ellipsoid terrain
        m_Provider = CreateRef<EllipsoidTerrainProvider>();
    }

    TerrainSystem::~TerrainSystem() {
        for (auto& pair : m_TileCache) {
            delete pair.second;
        }
    }

    void TerrainSystem::Initialize() {
        PT_EARTH_INFO("Terrain system initialized");
    }

    void TerrainSystem::Shutdown() {
    }

    float TerrainSystem::GetHeight(double longitude, double latitude) {
        if (!m_Provider) return 0.0f;
        
        Cartographic pos(longitude, latitude, 0.0);
        auto sample = m_Provider->SampleHeight(pos);
        return sample.IsValid ? sample.Height : 0.0f;
    }

    TerrainTile* TerrainSystem::GetTerrainTile(const TileID& id) {
        auto it = m_TileCache.find(id);
        if (it != m_TileCache.end()) {
            return it->second;
        }
        
        if (!m_Provider) return nullptr;
        
        auto* tile = m_Provider->GetTile(id);
        if (tile) {
            tile->GenerateMesh();
            m_TileCache[id] = tile;
        }
        
        return tile;
    }

    void TerrainSystem::Update() {
        // Process async terrain loading
    }

}
