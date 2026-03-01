#pragma once
#include "PotatoEarthBase.h"
#include "EarthCore/Event/Event.h"
#include "EarthCore/Render/GraphicsWindow.h"

namespace PTEarth {
    class WindowResizedEvent;
    class WindowCloseEvent;
    class Scene;
    class Camera;
    class TileQuadtree;

    class PTEARTH_API Application 
    {
    public:
        Application(const HDC& inHDC);
        virtual ~Application();

        void onEvent(Event& e);
        void Close();
        void Run();

        void OnUpdate();
        void OnRender();
        void OnImGuiRender();

        inline GraphicsWindow& GetGraphicWindow() { return *m_Window; }
        inline static Application& Get() { return *S_Instance; }

        PT_Ref<Scene> GetScene() { return m_Scene; }
        PT_Ref<Camera> GetCamera() { return m_Camera; }

    private:
        bool onWindowClose(WindowCloseEvent& e);
        bool onWindowResized(WindowResizedEvent& e);

        void InitializeScene();

    private:
        std::unique_ptr<GraphicsWindow> m_Window;
        bool m_Running = true;
        static Application* S_Instance;

        PT_Ref<Scene> m_Scene;
        PT_Ref<Camera> m_Camera;
        PT_Ref<TileQuadtree> m_TileQuadtree;

        float m_LastFrameTime = 0.0f;
    };

    Application* CreateApplication(const HDC& inHDC);
}
