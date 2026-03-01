#include "potatopch.h"
#include "Application.h"

#include "EarthCore/Event/WindowEvent.h"
#include "EarthCore/Event/KeyEvent.h"
#include "EarthCore/Event/MouseEvent.h"
#include "EarthCore/Core/PTInput.h"
#include "PTKeyCodes.h"
#include "PTButtonCodes.h"

#include "EarthCore/Scene/Scene.h"
#include "EarthCore/Scene/Entity.h"
#include "EarthCore/Scene/EarthGlobe.h"
#include "EarthCore/Render/Camera.h"
#include "EarthCore/Core/Log.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace PTEarth {
    Application* Application::S_Instance = nullptr;

    Application::Application(const HDC& inHDC)
    {
        PTEARTH_ENGINGE_ASSERT(!S_Instance, "App already exists!");
        S_Instance = this;

        // Create window
        m_Window = GraphicsWindow::Create(WindowProps(inHDC));
        m_Window->SetVSync(true);

        // Initialize scene
        InitializeScene();
    }

    Application::~Application()
    {
    }

    void Application::InitializeScene() {
        // Create scene
        m_Scene = CreateRef<Scene>();

        // Create camera
        float aspectRatio = 1280.0f / 720.0f; // Default aspect
        m_Camera = CreateRef<EarthOrbitCamera>(aspectRatio);
        m_Scene->SetActiveCamera(m_Camera);

        // Create Earth globe
        m_EarthGlobe = CreateRef<EarthGlobe>();
        m_EarthGlobe->Initialize();

        // Add Earth to scene
        Entity earthEntity = m_Scene->CreateEntity("Earth");
        earthEntity.SetTransform(TransformComponent(glm::mat4(1.0f)));
        
        // Create mesh for Earth
        MeshComponent earthMesh;
        earthMesh.VertexArray = m_EarthGlobe->GetVertexArray();
        earthMesh.Shader = m_EarthGlobe->GetShader();
        earthMesh.Color = glm::vec4(0.2f, 0.4f, 0.8f, 1.0f);
        earthEntity.SetMesh(earthMesh);

        PT_EARTH_INFO("Scene initialized with Earth");
    }

    void Application::onEvent(Event& e)
    {
        EventDispatcher dispatcher(e);

        dispatcher.Dispatch<WindowCloseEvent>(PT_BIND_EVENT_FN(Application::onWindowClose));
        dispatcher.Dispatch<WindowResizedEvent>(PT_BIND_EVENT_FN(Application::onWindowResized));

        m_Window->onWindowsEvent(e);
    }

    void Application::Close()
    {
        m_Running = false;
    }

    void Application::Run()
    {
        // Calculate delta time
        float time = (float)glfwGetTime();
        float deltaTime = time - m_LastFrameTime;
        m_LastFrameTime = time;

        // Update
        OnUpdate();

        // Render
        OnRender();
    }

    void Application::OnUpdate() {
        // Calculate delta time
        float time = (float)glfwGetTime();
        static float lastTime = 0.0f;
        float deltaTime = time - lastTime;
        lastTime = time;

        // Update scene
        if (m_Scene) {
            m_Scene->OnUpdate(deltaTime);
        }

        // Camera controls
        if (auto* orbitCam = dynamic_cast<EarthOrbitCamera*>(m_Camera.get())) {
            if (PTInput::IsKeyPressed((int)PTEarth_Key::Key_W)) {
                orbitCam->Zoom(-100000.0f);
            }
            if (PTInput::IsKeyPressed((int)PTEarth_Key::Key_S)) {
                orbitCam->Zoom(100000.0f);
            }
            if (PTInput::IsKeyPressed((int)PTEarth_Key::Key_A)) {
                orbitCam->Orbit(-0.01f, 0.0f);
            }
            if (PTInput::IsKeyPressed((int)PTEarth_Key::Key_D)) {
                orbitCam->Orbit(0.01f, 0.0f);
            }
        }
    }

    void Application::OnRender() {
        // Clear
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glEnable(GL_DEPTH_TEST);

        // Render scene
        if (m_Scene && m_Camera) {
            m_Scene->OnRender(*m_Camera);
        }

        // Swap buffers
        m_Window->OnUpdate();
    }

    void Application::OnImGuiRender() {
        // ImGui rendering can be added here
    }

    bool Application::onWindowClose(WindowCloseEvent& e)
    {
        m_Running = false;
        return true;
    }

    bool Application::onWindowResized(WindowResizedEvent& e)
    {
        if (e.GetWidth() == 0 || e.GetHeight() == 0)
        {
            return false;
        }

        glViewport(0, 0, e.GetWidth(), e.GetHeight());

        // Update camera aspect ratio
        if (auto* orbitCam = dynamic_cast<EarthOrbitCamera*>(m_Camera.get())) {
            orbitCam->SetViewportSize((float)e.GetWidth(), (float)e.GetHeight());
        }

        return false;
    }
}
