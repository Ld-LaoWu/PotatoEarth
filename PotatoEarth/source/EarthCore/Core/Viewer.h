#pragma once

#include "EarthCore/Core/PotatoEarthBase.h"
#include "EarthCore/Core/Application.h"
#include "EarthCore/Scene/Scene.h"
#include "EarthCore/Scene/TileQuadtree.h"
#include "EarthCore/Scene/Atmosphere.h"
#include "EarthCore/Scene/Entity.h"
#include "EarthCore/Render/Camera.h"
#include "EarthCore/Render/Shader.h"
#include "Clock.h"

namespace PTEarth {

    // Complete viewer class - similar to Cesium.Viewer
    class PTEARTH_API Viewer {
    public:
        Viewer(void* nativeWindowHandle);
        ~Viewer();
        
        // Initialize all systems
        void Initialize();
        void Shutdown();
        
        // Main loop
        void Run();
        void OnUpdate(float deltaTime);
        void OnRender();
        
        // Scene access
        PT_Ref<Scene> GetScene() { return m_Scene; }
        PT_Ref<Camera> GetCamera() { return m_Camera; }
        PT_Ref<Clock> GetClock() { return m_Clock; }
        
        // Globe/Earth
        PT_Ref<TileQuadtree> GetGlobe() { return m_Globe; }
        
        // Atmosphere
        PT_Ref<Atmosphere> GetAtmosphere() { return m_Atmosphere; }
        void SetShowAtmosphere(bool show) { m_ShowAtmosphere = show; }
        
        // Entities
        EntityCollection& GetEntities() { return m_Entities; }
        Entity* AddEntity(const std::string& id);
        void RemoveEntity(const std::string& id);
        
        // Camera control
        void FlyTo(const glm::dvec3& destination, float duration = 2.0f);
        void FlyTo(const Cartographic& destination, float duration = 2.0f);
        void ZoomTo(Entity* entity);
        
        // Imagery layers
        // TODO: Add imagery layer support
        
        // Terrain
        // TODO: Add terrain provider support
        
        // 3D Tiles
        // TODO: Add 3D tiles support
        
        // Rendering options
        void SetShowWireframe(bool show) { m_ShowWireframe = show; }
        void SetShowGlobe(bool show) { m_ShowGlobe = show; }
        void SetShowSky(bool show) { m_ShowSky = show; }
        
        // Screen space events
        // TODO: Add pick/selection support
        
        // Resize
        void OnResize(int width, int height);
        
    private:
        void InitializeSystems();
        void CreateDefaultScene();
        void UpdateCamera(float deltaTime);
        void RenderGlobe();
        void RenderAtmosphere();
        void RenderEntities();
        
    private:
        // Core systems
        std::unique_ptr<Application> m_Application;
        PT_Ref<Scene> m_Scene;
        PT_Ref<Camera> m_Camera;
        PT_Ref<Clock> m_Clock;
        
        // Globe
        PT_Ref<TileQuadtree> m_Globe;
        bool m_ShowGlobe = true;
        bool m_ShowWireframe = false;
        
        // Atmosphere
        PT_Ref<Atmosphere> m_Atmosphere;
        bool m_ShowAtmosphere = true;
        bool m_ShowSky = true;
        
        // Entities
        EntityCollection m_Entities;
        PT_Ref<Shader> m_EntityShader;
        
        // Window
        void* m_WindowHandle;
        int m_Width = 1280;
        int m_Height = 720;
        
        // Rendering
        PT_Ref<Shader> m_GlobeShader;
    };

    // Widgets
    class ViewerWidgets {
    public:
        // Navigation help
        bool ShowNavigationHelp = true;
        
        // Timeline
        bool ShowTimeline = true;
        
        // Animation
        bool ShowAnimation = true;
        
        // Base layer picker
        bool ShowBaseLayerPicker = true;
        
        // Fullscreen button
        bool ShowFullscreenButton = true;
        
        // Geocoder (search)
        bool ShowGeocoder = true;
        
        // Home button
        bool ShowHomeButton = true;
        
        // Scene mode picker
        bool ShowSceneModePicker = true;
        
        // Selection indicator
        bool ShowSelectionIndicator = true;
        
        // Distance legend
        bool ShowDistanceLegend = true;
        
        // Compass
        bool ShowCompass = true;
    };

}
