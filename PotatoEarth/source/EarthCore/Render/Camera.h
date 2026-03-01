#pragma once

#include "EarthCore/Core/PotatoEarthBase.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace PTEarth {

    class Camera {
    public:
        Camera() = default;
        Camera(const glm::mat4& projection)
            : m_Projection(projection) {}

        virtual ~Camera() = default;

        const glm::mat4& GetProjection() const { return m_Projection; }
        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        const glm::mat4& GetViewProjection() const { return m_ViewProjection; }

        virtual void RecalculateViewMatrix() = 0;

    protected:
        glm::mat4 m_Projection = glm::mat4(1.0f);
        glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
        glm::mat4 m_ViewProjection = glm::mat4(1.0f);
    };

    class PerspectiveCamera : public Camera {
    public:
        PerspectiveCamera(float fov, float aspectRatio, float nearClip, float farClip);

        void SetProjection(float fov, float aspectRatio, float nearClip, float farClip);

        const glm::vec3& GetPosition() const { return m_Position; }
        void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }

        const glm::vec3& GetRotation() const { return m_Rotation; }
        void SetRotation(const glm::vec3& rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }

        void RecalculateViewMatrix() override;

    private:
        glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
        glm::vec3 m_Rotation = { 0.0f, 0.0f, 0.0f };
    };

    // Earth orbit camera for geographic viewing
    class EarthOrbitCamera : public Camera {
    public:
        EarthOrbitCamera(float aspectRatio);

        void SetViewportSize(float width, float height) {
            m_AspectRatio = width / height;
            RecalculateProjection();
        }

        // Geographic positioning
        void SetCartographic(double longitude, double latitude, double height);
        void SetCartesian(const glm::dvec3& position);

        // Orbit controls
        void Orbit(float deltaLongitude, float deltaLatitude);
        void Zoom(float deltaHeight);
        void Rotate(float deltaHeading, float deltaPitch);

        void RecalculateViewMatrix() override;

        double GetLongitude() const { return m_Longitude; }
        double GetLatitude() const { return m_Latitude; }
        double GetHeight() const { return m_Height; }

    private:
        void RecalculateProjection();

    private:
        double m_Longitude = 0.0;  // radians
        double m_Latitude = 0.0;   // radians
        double m_Height = 20000000.0; // meters (default: 20000km)
        double m_Heading = 0.0;    // radians
        double m_Pitch = -PI / 2;  // radians (looking down)

        float m_FOV = 60.0f;
        float m_AspectRatio = 16.0f / 9.0f;
        float m_NearClip = 1.0f;
        float m_FarClip = 100000000.0f; // 100 million meters
    };

    constexpr double PI = 3.14159265358979323846;

}
