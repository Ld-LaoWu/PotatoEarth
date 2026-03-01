#pragma once

#include "EarthCore/Core/PotatoEarthBase.h"
#include "EarthCore/Render/Buffer.h"
#include "EarthCore/Render/Shader.h"
#include "EarthCore/Render/Material.h"
#include <glm/glm.hpp>
#include <vector>

namespace PTEarth {

    // Instance data for instanced rendering
    struct InstanceData {
        glm::mat4 Transform;
        glm::vec4 Color;
        uint32_t TextureIndex;
    };

    // Instanced mesh for efficient rendering of many objects
    class InstancedMesh {
    public:
        InstancedMesh();
        ~InstancedMesh();
        
        void SetMesh(const PT_Ref<VertexArray>& mesh) { m_Mesh = mesh; }
        void SetMaterial(const PT_Ref<Material>& material) { m_Material = material; }
        
        void SetInstances(const std::vector<InstanceData>& instances);
        void UpdateInstances(const std::vector<InstanceData&>& instances, size_t startIndex);
        
        void Render(const glm::mat4& viewProjection, const PT_Ref<Shader>& shader);
        
        uint32_t GetInstanceCount() const { return m_InstanceCount; }
        
    private:
        void CreateInstanceBuffer();
        void UpdateBuffer();
        
    private:
        PT_Ref<VertexArray> m_Mesh;
        PT_Ref<Material> m_Material;
        PT_Ref<VertexBuffer> m_InstanceBuffer;
        
        std::vector<InstanceData> m_Instances;
        uint32_t m_InstanceCount = 0;
        uint32_t m_Capacity = 0;
        bool m_Dirty = false;
    };

    // Batch renderer for static geometry
    class BatchRenderer {
    public:
        void Initialize();
        void Shutdown();
        
        // Add mesh to batch
        void Submit(const PT_Ref<VertexArray>& mesh, const glm::mat4& transform, const glm::vec4& color);
        void Submit(const PT_Ref<VertexArray>& mesh, const PT_Ref<Material>& material, const glm::mat4& transform);
        
        // Flush all batched geometry
        void Flush(const glm::mat4& viewProjection, const PT_Ref<Shader>& shader);
        
        void Clear();
        
    private:
        struct BatchItem {
            PT_Ref<VertexArray> Mesh;
            PT_Ref<Material> Material;
            glm::mat4 Transform;
            glm::vec4 Color;
        };
        
        std::vector<BatchItem> m_Items;
        PT_Ref<Shader> m_DefaultShader;
    };

    // LOD system for geometry
    class LODSystem {
    public:
        struct LODLevel {
            float Distance;
            PT_Ref<VertexArray> Mesh;
            uint32_t VertexCount;
        };
        
        void AddLODLevel(float distance, const PT_Ref<VertexArray>& mesh);
        PT_Ref<VertexArray> GetLODForDistance(float distance) const;
        
    private:
        std::vector<LODLevel> m_LODLevels;
    };

    // Occlusion culling
    class OcclusionCulling {
    public:
        void Initialize();
        void Shutdown();
        
        // Begin occlusion query for object
        void BeginQuery(uint32_t objectId);
        void EndQuery(uint32_t objectId);
        
        // Check if object was visible last frame
        bool WasVisible(uint32_t objectId) const;
        
        // Update queries (call at end of frame)
        void Update();
        
        // Render depth pre-pass
        void RenderDepthPrePass(const std::vector<class Entity*>& entities, const glm::mat4& viewProjection);
        
    private:
        std::unordered_map<uint32_t, uint32_t> m_Queries;  // objectId -> queryId
        std::unordered_map<uint32_t, bool> m_Visibility;
    };

    // GPU profiler
    class GPUProfiler {
    public:
        void Initialize();
        void Shutdown();
        
        void BeginFrame();
        void EndFrame();
        
        void BeginQuery(const std::string& name);
        void EndQuery(const std::string& name);
        
        float GetElapsedTime(const std::string& name) const;
        
        struct ProfileResult {
            std::string Name;
            float Time;
        };
        std::vector<ProfileResult> GetResults() const;
        
    private:
        struct Query {
            uint32_t StartQuery;
            uint32_t EndQuery;
            float ElapsedTime;
        };
        
        std::unordered_map<std::string, Query> m_Queries;
        std::string m_CurrentQuery;
        
        uint32_t m_QueryPool = 0;
        uint32_t m_FrameQuery = 0;
    };

    // Memory allocator for GPU resources
    class GPUMemoryPool {
    public:
        struct Allocation {
            uint32_t BufferID;
            size_t Offset;
            size_t Size;
        };
        
        void Initialize(size_t poolSize);
        void Shutdown();
        
        Allocation Allocate(size_t size);
        void Free(const Allocation& allocation);
        
        void Defragment();
        
        size_t GetFreeSpace() const;
        size_t GetTotalSize() const { return m_PoolSize; }
        
    private:
        size_t m_PoolSize = 0;
        uint32_t m_BufferID = 0;
        
        struct FreeBlock {
            size_t Offset;
            size_t Size;
        };
        std::vector<FreeBlock> m_FreeBlocks;
    };

}
