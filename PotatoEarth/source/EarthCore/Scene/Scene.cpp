#include "potatopch.h"
#include "Scene.h"
#include "Entity.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

namespace PTEarth {

    Scene::Scene() {
    }

    Scene::~Scene() {
    }

    Entity Scene::CreateEntity(const std::string& name) {
        uint32_t id = (uint32_t)m_Entities.size();
        Entity entity(id, this);
        entity.SetTag(TagComponent(name.empty() ? "Entity" : name));
        m_Entities.push_back(entity);
        return entity;
    }

    void Scene::DestroyEntity(Entity entity) {
        // Simplified - in a real ECS this would be more complex
        // For now, we just mark as invalid or remove from vector
    }

    void Scene::OnUpdate(float deltaTime) {
        // Update all entities
        for (auto& entity : m_Entities) {
            // Update logic here
        }
    }

    void Scene::OnRender(Camera& camera) {
        if (!m_ActiveCamera) {
            PT_EARTH_WARN("No active camera set for scene rendering");
            return;
        }

        // Render all entities with mesh components
        for (auto& entity : m_Entities) {
            if (!entity.HasMesh()) continue;

            auto& mesh = entity.GetMesh();
            auto& transform = entity.GetTransform();

            if (mesh.Shader) {
                mesh.Shader->Bind();
                mesh.Shader->SetMat4("u_ViewProjection", camera.GetViewProjection());
                mesh.Shader->SetMat4("u_Transform", transform.Transform);
                mesh.Shader->SetFloat4("u_Color", mesh.Color);
            }

            if (mesh.VertexArray) {
                mesh.VertexArray->Bind();
                if (mesh.VertexArray->GetIndexBuffer()) {
                    glDrawElements(GL_TRIANGLES, mesh.VertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
                } else {
                    // Non-indexed drawing - would need vertex count
                }
            }
        }
    }

}
