#pragma once

#include "EarthCore/Core/PotatoEarthBase.h"
#include "EarthCore/Render/Camera.h"
#include <glm/glm.hpp>

namespace PTEarth {

    // VR rendering modes
    enum class VRMode {
        None,
        VR,      // Full VR (headset required)
        Stereo   // Side-by-side stereo
    };

    // VR device info
    struct VRDeviceInfo {
        std::string Name;
        int RenderWidth;
        int RenderHeight;
        float RefreshRate;
        float IPD;  // Inter-pupillary distance
    };

    // VR system interface
    class VRSystem {
    public:
        virtual ~VRSystem() = default;
        
        // Initialize VR
        virtual bool Initialize() = 0;
        virtual void Shutdown() = 0;
        
        // Device info
        virtual VRDeviceInfo GetDeviceInfo() const = 0;
        
        // Rendering
        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;
        
        // Eye rendering
        virtual void BeginEye(int eyeIndex) = 0;  // 0 = left, 1 = right
        virtual void EndEye(int eyeIndex) = 0;
        
        // Get eye camera
        virtual Camera GetEyeCamera(int eyeIndex, const Camera& baseCamera) = 0;
        
        // Tracking
        virtual glm::mat4 GetHeadTransform() const = 0;
        virtual glm::mat4 GetControllerTransform(int controllerIndex) const = 0;
        
        // Input
        virtual bool IsButtonPressed(int controllerIndex, int button) const = 0;
        virtual glm::vec2 GetControllerThumbstick(int controllerIndex) const = 0;
        
        // Available
        virtual bool IsAvailable() const = 0;
    };

    // OpenVR implementation
    class OpenVRSystem : public VRSystem {
    public:
        bool Initialize() override;
        void Shutdown() override;
        
        VRDeviceInfo GetDeviceInfo() const override;
        void BeginFrame() override;
        void EndFrame() override;
        void BeginEye(int eyeIndex) override;
        void EndEye(int eyeIndex) override;
        Camera GetEyeCamera(int eyeIndex, const Camera& baseCamera) override;
        glm::mat4 GetHeadTransform() const override;
        glm::mat4 GetControllerTransform(int controllerIndex) const override;
        bool IsButtonPressed(int controllerIndex, int button) const override;
        glm::vec2 GetControllerThumbstick(int controllerIndex) const override;
        bool IsAvailable() const override;
        
    private:
        void* m_VRSystem = nullptr;
        void* m_VRCompositor = nullptr;
        bool m_Initialized = false;
    };

    // AR system interface
    class ARSystem {
    public:
        virtual ~ARSystem() = default;
        
        virtual bool Initialize() = 0;
        virtual void Shutdown() = 0;
        
        // Camera feed
        virtual void UpdateCameraFeed() = 0;
        virtual PT_Ref<Texture> GetCameraFeedTexture() = 0;
        
        // Tracking
        virtual glm::mat4 GetWorldTransform() const = 0;
        virtual bool IsTracking() const = 0;
        
        // Plane detection
        struct DetectedPlane {
            glm::vec3 Center;
            glm::vec3 Normal;
            glm::vec2 Extents;
            std::vector<glm::vec3> Vertices;
        };
        virtual std::vector<DetectedPlane> GetDetectedPlanes() const = 0;
        
        // Hit test
        struct HitResult {
            bool Valid;
            glm::vec3 Position;
            glm::vec3 Normal;
        };
        virtual HitResult PerformHitTest(const glm::vec2& screenPos) = 0;
        
        virtual bool IsAvailable() const = 0;
    };

    // XR manager
    class XRManager {
    public:
        void Initialize();
        void Shutdown();
        
        void SetVRMode(VRMode mode) { m_VRMode = mode; }
        VRMode GetVRMode() const { return m_VRMode; }
        
        void SetVRSystem(PT_Ref<VRSystem> system) { m_VRSystem = system; }
        void SetARSystem(PT_Ref<ARSystem> system) { m_ARSystem = system; }
        
        PT_Ref<VRSystem> GetVRSystem() const { return m_VRSystem; }
        PT_Ref<ARSystem> GetARSystem() const { return m_ARSystem; }
        
        bool IsVRActive() const { return m_VRMode != VRMode::None && m_VRSystem; }
        bool IsARActive() const { return m_ARSystem != nullptr; }
        
        // Render VR frame
        void RenderVR(class Viewer* viewer);
        
        // Render AR overlay
        void RenderAR(class Viewer* viewer);
        
    private:
        VRMode m_VRMode = VRMode::None;
        PT_Ref<VRSystem> m_VRSystem;
        PT_Ref<ARSystem> m_ARSystem;
    };

}
