#include "potatopch.h"
#include "TileQuadtree.h"

#include "EarthCore/Core/Geospatial/Ellipsoid.h"
#include <glm/gtc/matrix_transform.hpp>

namespace PTEarth {

    TileQuadtree::TileQuadtree() {
    }

    TileQuadtree::~TileQuadtree() {
        delete m_Root;
    }

    void TileQuadtree::Initialize() {
        // Create root tile (level 0, covers entire world)
        m_Root = new TileNode(TileID(0, 0, 0));
        
        // Initialize root tile
        m_Root->TileData = CreateRef<Tile>(m_Root->ID);
        m_Root->TileData->Initialize();
        m_TileCache[m_Root->ID] = m_Root->TileData;
    }

    void TileQuadtree::Update(const Camera& camera) {
        if (!m_Root) return;

        // Update tree starting from root
        UpdateNode(m_Root, camera);

        // Process loading queue
        size_t tilesToLoad = glm::min(m_LoadQueue.size(), m_MaxTilesPerFrame);
        for (size_t i = 0; i < tilesToLoad; ++i) {
            TileID id = m_LoadQueue.front();
            m_LoadQueue.pop();

            if (m_TileCache.find(id) == m_TileCache.end()) {
                auto tile = CreateRef<Tile>(id);
                tile->Initialize();
                m_TileCache[id] = tile;
            }
        }
    }

    void TileQuadtree::UpdateNode(TileNode* node, const Camera& camera) {
        if (!node) return;

        // Calculate visibility
        // For now, simple frustum culling could be added here
        node->IsVisible = true;

        bool shouldRefine = ShouldRefine(node, camera);

        if (shouldRefine && node->ID.Level < m_MaxLevel) {
            // Create children if needed
            if (node->Children.empty()) {
                CreateChildren(node);
            }

            // Recursively update children
            for (auto* child : node->Children) {
                UpdateNode(child, camera);
            }
        } else {
            // Destroy children if we don't need them anymore
            if (!node->Children.empty()) {
                DestroyChildren(node);
            }
        }
    }

    bool TileQuadtree::ShouldRefine(TileNode* node, const Camera& camera) {
        // Calculate geometric error at current camera distance
        // This is a simplified version - real implementation would use more sophisticated error metrics

        auto* earthCam = dynamic_cast<const EarthOrbitCamera*>(&camera);
        if (!earthCam) return false;

        // Get tile center
        Cartographic sw = node->TileData->GetSouthwest();
        Cartographic ne = node->TileData->GetNortheast();
        
        double centerLon = (sw.longitude + ne.longitude) * 0.5;
        double centerLat = (sw.latitude + ne.latitude) * 0.5;
        
        Cartographic centerCarto(centerLon, centerLat, 0.0);
        glm::dvec3 centerPos = Ellipsoid::WGS84.cartographicToCartesian(centerCarto);

        // Camera position
        glm::dvec3 camPos = Ellipsoid::WGS84.cartographicToCartesian(
            Cartographic(earthCam->GetLongitude(), earthCam->GetLatitude(), earthCam->GetHeight())
        );

        // Distance from camera to tile center
        double distance = glm::length(centerPos - camPos);

        // Tile size approximation (at equator)
        double tileSize = 40075000.0 / (1 << node->ID.Level); // Earth's circumference / tiles at level

        // Screen space error approximation
        // When closer than tile size * 2, we should refine
        double screenError = tileSize / (distance + 1.0) * 1000.0;

        return screenError > m_ScreenSpaceError;
    }

    void TileQuadtree::CreateChildren(TileNode* node) {
        if (!node || !node->TileData) return;

        for (int i = 0; i < 4; ++i) {
            TileID childID = node->TileData->GetChild(i);
            
            TileNode* childNode = new TileNode(childID);
            childNode->Parent = node;

            // Get or create tile data
            if (m_TileCache.find(childID) != m_TileCache.end()) {
                childNode->TileData = m_TileCache[childID];
            } else {
                m_LoadQueue.push(childID);
            }

            node->Children.push_back(childNode);
        }
    }

    void TileQuadtree::DestroyChildren(TileNode* node) {
        if (!node) return;

        for (auto* child : node->Children) {
            delete child;
        }
        node->Children.clear();
    }

    void TileQuadtree::Render(const glm::mat4& viewProjection, const PT_Ref<Shader>& shader) {
        if (!m_Root) return;
        RenderNode(m_Root, viewProjection, shader);
    }

    void TileQuadtree::RenderNode(TileNode* node, const glm::mat4& viewProjection, const PT_Ref<Shader>& shader) {
        if (!node || !node->IsVisible) return;

        // If has visible children, render them instead
        bool hasVisibleChildren = false;
        for (auto* child : node->Children) {
            if (child->IsVisible) {
                hasVisibleChildren = true;
                break;
            }
        }

        if (hasVisibleChildren) {
            for (auto* child : node->Children) {
                RenderNode(child, viewProjection, shader);
            }
        } else if (node->TileData && node->TileData->GetState() == TileState::Loaded) {
            // Render this tile
            node->TileData->Render(viewProjection, shader);
        }
    }

}
