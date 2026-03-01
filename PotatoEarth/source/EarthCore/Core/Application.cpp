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
#include "EarthCore/Scene/TileQuadtree.h"
#include "EarthCore/Render/Camera.h"
#include "EarthCore/Core/Log.h"

#include <glad/glad.h>


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
        
        // Set initial camera position (over China)
        auto* orbitCam = static_cast<EarthOrbitCamera*>(m_Camera.get());
        orbitCam->SetCartographic(glm::radians(104.0), glm::radians(35.0), 20000000.0);
        
        m_Scene->SetActiveCamera(m_Camera);

        // Create tile quadtree for Earth
        m_TileQuadtree = CreateRef<TileQuadtree>();
        m_TileQuadtree->Initialize();

        PT_EARTH_INFO("Scene initialized with TileQuadtree Earth");
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
        float time = (float)(float)GetTickCount() / 1000.0f;
        float deltaTime = time - m_LastFrameTime;
        m_LastFrameTime = time;

        // Update
        OnUpdate();

        // Render
        OnRender();
    }

    void Application::OnUpdate() {
        // Calculate delta time
        float time = (float)(float)GetTickCount() / 1000.0f;
        static float lastTime = 0.0f;
        float deltaTime = time - lastTime;
        lastTime = time;

        // Update camera controls
        if (auto* orbitCam = dynamic_cast<EarthOrbitCamera*>(m_Camera.get())) {
            // Orbit
            if (PTInput::IsKeyPressed((int)PTEarth_Key::Key_A)) {
                orbitCam->Orbit(0.005f, 0.0f);
            }
            if (PTInput::IsKeyPressed((int)PTEarth_Key::Key_D)) {
                orbitCam->Orbit(-0.005f, 0.0f);
            }
            if (PTInput::IsKeyPressed((int)PTEarth_Key::Key_W)) {
                orbitCam->Orbit(0.0f, 0.005f);
            }
            if (PTInput::IsKeyPressed((int)PTEarth_Key::Key_S)) {
                orbitCam->Orbit(0.0f, -0.005f);
            }
            
            // Zoom
            if (PTInput::IsKeyPressed((int)PTEarth_Key::Key_Q)) {
                orbitCam->Zoom(-1000000.0f);
            }
            if (PTInput::IsKeyPressed((int)PTEarth_Key::Key_E)) {
                orbitCam->Zoom(1000000.0f);
            }
            
            // Rotate view
            if (PTInput::IsKeyPressed((int)PTEarth_Key::Key_Left)) {
                orbitCam->Rotate(0.02f, 0.0f);
            }
            if (PTInput::IsKeyPressed((int)PTEarth_Key::Key_Right)) {
                orbitCam->Rotate(-0.02f, 0.0f);
            }
            if (PTInput::IsKeyPressed((int)PTEarth_Key::Key_Up)) {
                orbitCam->Rotate(0.0f, -0.02f);
            }
            if (PTInput::IsKeyPressed((int)PTEarth_Key::Key_Down)) {
                orbitCam->Rotate(0.0f, 0.02f);
            }
        }

        // Update tile quadtree
        if (m_TileQuadtree && m_Camera) {
            m_TileQuadtree->Update(*m_Camera);
        }

        // Update scene
        if (m_Scene) {
            m_Scene->OnUpdate(deltaTime);
        }
    }

    void Application::OnRender() {
        // Clear
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.05f, 0.05f, 0.1f, 1.0f); // Dark blue space
        glEnable(GL_DEPTH_TEST);

        // Render tile quadtree
        if (m_TileQuadtree && m_Camera) {
            // Create a basic shader for tiles if needed
            static PT_Ref<Shader> tileShader;
            if (!tileShader) {
                std::string vertexSrc = R"(
                    #version 330 core
                    layout(location = 0) in vec3 a_Position;
                    layout(location = 1) in vec3 a_Normal;
                    layout(location = 2) in vec2 a_TexCoord;
                    
                    uniform mat4 u_ViewProjection;
                    uniform mat4 u_Transform;
                    
                    out vec3 v_Normal;
                    out vec2 v_TexCoord;
                    
                    void main() {
                        v_Normal = mat3(transpose(inverse(u_Transform))) * a_Normal;
                        v_TexCoord = a_TexCoord;
                        gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
                    }
                )";

                std::string fragmentSrc = R"(
                    #version 330 core
                    layout(location = 0) out vec4 o_Color;
                    
                    in vec3 v_Normal;
                    in vec2 v_TexCoord;
                    
                    void main() {
                        // Simple earth-like coloring based on UV
                        vec3 oceanColor = vec3(0.1, 0.3, 0.6);
                        vec3 landColor = vec3(0.2, 0.5, 0.2);
                        
                        // Simple lighting
                        vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
                        float diff = max(dot(normalize(v_Normal), lightDir), 0.0);
                        vec3 ambient = vec3(0.05, 0.05, 0.1);
                        
                        // Grid pattern for debugging
                        float gridX = step(0.95, fract(v_TexCoord.x * 10.0));
                        float gridY = step(0.95, fract(v_TexCoord.y * 10.0));
                        vec3 gridColor = vec3(0.3, 0.3, 0.3);
                        
                        vec3 baseColor = mix(oceanColor, gridColor, max(gridX, gridY) * 0.3);
                        vec3 result = ambient + baseColor * diff;
                        
                        o_Color = vec4(result, 1.0);
                    }
                )";
                tileShader = Shader::Create("TileShader", vertexSrc, fragmentSrc);
            }
            
            m_TileQuadtree->Render(m_Camera->GetViewProjection(), tileShader);
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
