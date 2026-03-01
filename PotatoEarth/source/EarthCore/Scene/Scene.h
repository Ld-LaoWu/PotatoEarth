#pragma once

#include "EarthCore/Core/PotatoEarthBase.h"
#include "EarthCore/Render/Camera.h"
#include "EarthCore/Render/Shader.h"

#include <glm/glm.hpp>
#include <vector>

namespace PTEarth {

    class Entity;

    class Scene {
    public:
        Scene();
        ~Scene();

        Entity* CreateEntity(const std::string& name = std::string());
        void DestroyEntity(Entity* entity);

        void OnUpdate(float deltaTime);
        void OnRender(Camera& camera);

        PT_Ref<Camera> GetActiveCamera() { return m_ActiveCamera; }
        void SetActiveCamera(PT_Ref<Camera> camera) { m_ActiveCamera = camera; }

    private:
        std::vector<PT_Ref<Entity>> m_Entities;
        PT_Ref<Camera> m_ActiveCamera;
    };

}
