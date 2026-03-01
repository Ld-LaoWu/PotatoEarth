#pragma once

#include "EarthCore/Core/PotatoEarthBase.h"
#include "Tile.h"
#include "EarthCore/Render/Camera.h"
#include <glm/glm.hpp>
#include <unordered_map>
#include <queue>

namespace PTEarth {

    struct TileNode {
        TileID ID;
        PT_Ref<Tile> TileData;
        std::vector<TileNode*> Children;
        TileNode* Parent;
        bool IsVisible;

        TileNode(const TileID& id) : ID(id), Parent(nullptr), IsVisible(false) {}
        ~TileNode() {
            for (auto* child : Children) {
                delete child;
            }
        }
    };

    class TileQuadtree {
    public:
        TileQuadtree();
        ~TileQuadtree();

        void Initialize();
        void Update(const Camera& camera);
        void Render(const glm::mat4& viewProjection, const PT_Ref<Shader>& shader);

        // Configuration
        void SetMaxLevel(int maxLevel) { m_MaxLevel = maxLevel; }
        void SetScreenSpaceError(float error) { m_ScreenSpaceError = error; }

    private:
        void UpdateNode(TileNode* node, const Camera& camera);
        bool ShouldRefine(TileNode* node, const Camera& camera);
        void RenderNode(TileNode* node, const glm::mat4& viewProjection, const PT_Ref<Shader>& shader);
        void CreateChildren(TileNode* node);
        void DestroyChildren(TileNode* node);

    private:
        TileNode* m_Root;
        int m_MaxLevel = 18; // Maximum zoom level
        float m_ScreenSpaceError = 16.0f; // Pixels of error before refining

        // Tile cache
        std::unordered_map<TileID, PT_Ref<Tile>, TileIDHash> m_TileCache;

        // Loading queue
        std::queue<TileID> m_LoadQueue;
        const size_t m_MaxTilesPerFrame = 4;
    };

}
