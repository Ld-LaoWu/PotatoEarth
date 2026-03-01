#pragma once

#include "EarthCore/Core/PotatoEarthBase.h"
#include "EarthCore/Render/VertexArray.h"
#include "EarthCore/Render/Shader.h"
#include <glm/glm.hpp>

namespace PTEarth {

    struct TransformComponent {
        glm::mat4 Transform = glm::mat4(1.0f);
        TransformComponent() = default;
        TransformComponent(const glm::mat4& transform) : Transform(transform) {}
        operator glm::mat4& () { return Transform; }
        operator const glm::mat4& () const { return Transform; }
    };

    struct MeshComponent {
        PT_Ref<VertexArray> VertexArray;
        PT_Ref<Shader> Shader;
        glm::vec4 Color = glm::vec4(1.0f);
        MeshComponent() = default;
    };

    struct TagComponent {
        std::string Tag;
        TagComponent() = default;
        TagComponent(const std::string& tag) : Tag(tag) {}
    };

    class Entity {
    public:
        Entity() = default;
        Entity(uint32_t id, class Scene* scene) : m_EntityID(id), m_Scene(scene) {}

        uint32_t GetID() const { return m_EntityID; }
        TransformComponent& GetTransform() { return m_Transform; }
        MeshComponent& GetMesh() { return m_Mesh; }
        bool HasMesh() const { return m_Mesh.VertexArray != nullptr; }
        TagComponent& GetTag() { return m_Tag; }

        void SetTransform(const TransformComponent& transform) { m_Transform = transform; }
        void SetMesh(const MeshComponent& mesh) { m_Mesh = mesh; }
        void SetTag(const TagComponent& tag) { m_Tag = tag; }

    private:
        uint32_t m_EntityID = 0;
        class Scene* m_Scene = nullptr;
        TransformComponent m_Transform;
        MeshComponent m_Mesh;
        TagComponent m_Tag;
    };

}