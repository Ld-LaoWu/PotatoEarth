#pragma once

#include "EarthCore/Core/PotatoEarthBase.h"
#include "EarthCore/Render/VertexArray.h"
#include "EarthCore/Render/Material.h"
#include "EarthCore/Render/Shader.h"
#include "EarthCore/Scene/Tile.h"
#include <glm/glm.hpp>
#include <vector>

namespace PTEarth {

    // 3D Tiles format support
    // https://github.com/CesiumGS/3d-tiles

    enum class TileFormat {
        B3DM,  // Batched 3D Model
        I3DM,  // Instanced 3D Model
        PNTS,  // Point Cloud
        CMPT   // Composite
    };

    // glTF Model for 3D Tiles
    struct GLTFModel {
        std::vector<float> Positions;
        std::vector<float> Normals;
        std::vector<float> TexCoords;
        std::vector<uint32_t> Indices;
        std::vector<uint8_t> TextureData;
        
        // glTF extensions
        bool HasDracoCompression = false;
        bool HasKTX2Textures = false;
        
        // Load from glTF/glb
        static PT_Ref<GLTFModel> Load(const std::string& path);
        static PT_Ref<GLTFModel> LoadFromMemory(const std::vector<uint8_t>& data);
    };

    // 3D Tile content
    class Tile3DContent {
    public:
        TileFormat Format;
        std::vector<uint8_t> RawData;
        
        // Parsed content
        PT_Ref<GLTFModel> Model;
        PT_Ref<VertexArray> Mesh;
        PT_Ref<Material> Material;
        
        // Bounding volume
        struct BoundingVolume {
            glm::dvec3 Center;
            double Radius;  // For sphere
            // Or box, region
        } BoundingVolume;
        
        // Load and parse
        void Parse();
        void UploadToGPU();
        
        // Render
        void Render(const glm::mat4& viewProjection, const PT_Ref<Shader>& shader);
    };

    // 3D Tile node in tree
    struct Tile3DNode {
        std::string ID;
        
        // Bounding volume
        Tile3DContent::BoundingVolume BoundingVolume;
        
        // Geometric error
        float GeometricError = 0.0f;
        
        // Content
        PT_Ref<Tile3DContent> Content;
        
        // Refine strategy
        enum class Refine {
            REPLACE,    // Replace parent with children
            ADD         // Add children to parent
        } Refine = Refine::REPLACE;
        
        // Children
        std::vector<PT_Ref<Tile3DNode>> Children;
        Tile3DNode* Parent = nullptr;
        
        // Transform
        glm::dmat4 Transform = glm::dmat4(1.0);
        
        // Loading state
        bool ContentLoaded = false;
        bool ContentLoading = false;
    };

    // 3D Tileset
    class Tileset3D {
    public:
        struct Asset {
            std::string Version = "1.0";
            std::string TilesetVersion;
        } Asset;
        
        struct Properties {
            float GeometricError;
            std::string RootURL;
        } Properties;
        
        // Load from URL or file
        void Load(const std::string& url);
        
        // Update (traverse and load tiles)
        void Update(Camera& camera);
        
        // Render
        void Render(const glm::mat4& viewProjection);
        
        // Root tile
        PT_Ref<Tile3DNode> Root;
        
    private:
        void Traverse(Tile3DNode* node, Camera& camera);
        bool ShouldRefine(Tile3DNode* node, Camera& camera);
        void LoadTile(Tile3DNode* node);
        void UnloadTile(Tile3DNode* node);
        
        std::string m_BaseURL;
        PT_Ref<Shader> m_Shader;
    };

    // 3D Tiles styles
    class TileStyle {
    public:
        // CSS-like styling expressions
        // e.g., "color('red')", "${Height} * 0.5"
        
        struct ColorExpression {
            enum Type { CONSTANT, PROPERTY, MIX };
            Type ExprType;
            glm::vec4 Color;
            std::string PropertyName;
        };
        
        struct ShowExpression {
            enum Type { ALWAYS, CONDITION };
            Type ExprType;
            std::string Condition;  // e.g., "${Height} > 100"
        };
        
        ColorExpression Color;
        ShowExpression Show;
        
        // Parse from JSON
        static TileStyle Parse(const std::string& json);
        
        // Apply to tile
        void Apply(Tile3DContent* content);
    };

}
