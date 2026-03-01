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

    Entity* Scene::CreateEntity(const std::string& name) {
        auto entity = CreateRef<Entity>(name.empty() ? "Entity" : name);
        entity->SetName(name);
        m_Entities.push_back(entity);
        return entity.get();
    }

    void Scene::DestroyEntity(Entity* entity) {
        // Simplified
    }

    void Scene::OnUpdate(float deltaTime) {
        for (auto& entity : m_Entities) {
            // Update logic
        }
    }

    void Scene::OnRender(Camera& camera) {
        if (!m_ActiveCamera) {
            PT_EARTH_WARN("No active camera");
            return;
        }

        // TODO: Implement entity rendering
        // Entities need mesh component to be rendered
    }
}
