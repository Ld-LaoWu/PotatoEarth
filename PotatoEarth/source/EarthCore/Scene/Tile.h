#pragma once

#include "EarthCore/Core/PotatoEarthBase.h"
#include "EarthCore/Core/Geospatial/Cartograph.h"
#include "EarthCore/Render/VertexArray.h"
#include "EarthCore/Render/Shader.h"
#include <glm/glm.hpp>

namespace PTEarth {

    // Tile ID structure for quadtree addressing
    struct TileID {
        int Level;      // Zoom level (0 = root, 1 = 4 tiles, etc.)
        int X;          // X coordinate in level
        int Y;          // Y coordinate in level

        TileID() : Level(0), X(0), Y(0) {}
        TileID(int level, int x, int y) : Level(level), X(x), Y(y) {}

        bool operator==(const TileID& other) const {
            return Level == other.Level && X == other.X && Y == other.Y;
        }
    };

    // Hash function for TileID to use in unordered_map
    struct TileIDHash {
        size_t operator()(const TileID& id) const {
            return std::hash<int>()(id.Level) ^ 
                   (std::hash<int>()(id.X) << 1) ^ 
                   (std::hash<int>()(id.Y) << 2);
        }
    };

    enum class TileState {
        Unloaded,
        Loading,
        Loaded,
        Failed
    };

    class Tile {
    public:
        Tile(const TileID& id);
        ~Tile();

        void Initialize();
        void Render(const glm::mat4& viewProjection, const PT_Ref<Shader>& shader);

        // Geographic bounds
        void CalculateBounds();
        Cartographic GetSouthwest() const { return m_Southwest; }
        Cartographic GetNortheast() const { return m_Northeast; }

        // State
        TileState GetState() const { return m_State; }
        void SetState(TileState state) { m_State = state; }

        // ID
        TileID GetID() const { return m_ID; }

        // Children tiles (for quadtree)
        TileID GetChild(int index) const; // index: 0=SW, 1=SE, 2=NW, 3=NE
        bool HasChildren() const { return m_ID.Level < 20; } // Max level

    private:
        void GenerateMesh();

    private:
        TileID m_ID;
        TileState m_State = TileState::Unloaded;

        // Geographic bounds
        Cartographic m_Southwest;
        Cartographic m_Northeast;

        // Render data
        PT_Ref<VertexArray> m_VertexArray;
        PT_Ref<Shader> m_Shader;
    };

}
