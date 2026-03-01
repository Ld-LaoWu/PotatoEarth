#include "potatopch.h"
#include "Camera.h"

#include "EarthCore/Core/Geospatial/Ellipsoid.h"
#include "EarthCore/Core/Utility/Math/PTMath.h"

namespace PTEarth {

    // PerspectiveCamera //
    PerspectiveCamera::PerspectiveCamera(float fov, float aspectRatio, float nearClip, float farClip) {
        SetProjection(fov, aspectRatio, nearClip, farClip);
    }

    void PerspectiveCamera::SetProjection(float fov, float aspectRatio, float nearClip, float farClip) {
        m_Projection = glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
        m_ViewProjection = m_Projection * m_ViewMatrix;
    }

    void PerspectiveCamera::RecalculateViewMatrix() {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) *
            glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.x), glm::vec3(1, 0, 0)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.y), glm::vec3(0, 1, 0)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.z), glm::vec3(0, 0, 1));

        m_ViewMatrix = glm::inverse(transform);
        m_ViewProjection = m_Projection * m_ViewMatrix;
    }

    // EarthOrbitCamera //
    EarthOrbitCamera::EarthOrbitCamera(float aspectRatio)
        : m_AspectRatio(aspectRatio) {
        RecalculateProjection();
        RecalculateViewMatrix();
    }

    void EarthOrbitCamera::SetCartographic(double longitude, double latitude, double height) {
        m_Longitude = longitude;
        m_Latitude = latitude;
        m_Height = height;
        RecalculateViewMatrix();
    }

    void EarthOrbitCamera::SetCartesian(const glm::dvec3& position) {
        auto cartographic = Ellipsoid::WGS84.cartesianToCartographic(position);
        if (cartographic) {
            m_Longitude = cartographic->longitude;
            m_Latitude = cartographic->latitude;
            m_Height = cartographic->height;
            RecalculateViewMatrix();
        }
    }

    void EarthOrbitCamera::Orbit(float deltaLongitude, float deltaLatitude) {
        m_Longitude += deltaLongitude;
        m_Latitude += deltaLatitude;
        // Clamp latitude to avoid gimbal lock
        m_Latitude = glm::clamp(m_Latitude, -PTMath::OnePi / 2 + 0.01, PTMath::OnePi / 2 - 0.01);
        RecalculateViewMatrix();
    }

    void EarthOrbitCamera::Zoom(float deltaHeight) {
        m_Height = glm::max(100.0, m_Height + deltaHeight);
        RecalculateViewMatrix();
    }

    void EarthOrbitCamera::Rotate(float deltaHeading, float deltaPitch) {
        m_Heading += deltaHeading;
        m_Pitch += deltaPitch;
        // Clamp pitch
        m_Pitch = glm::clamp(m_Pitch, -PTMath::OnePi / 2 + 0.01, 0.0);
        RecalculateViewMatrix();
    }

    void EarthOrbitCamera::RecalculateViewMatrix() {
        // Get camera position in Cartesian (WGS84)
        Cartographic carto(m_Longitude, m_Latitude, m_Height);
        glm::dvec3 position = Ellipsoid::WGS84.cartographicToCartesian(carto);

        // Calculate target (center of earth)
        glm::dvec3 target(0.0, 0.0, 0.0);

        // Calculate up vector (surface normal at camera position)
        glm::dvec3 up = Ellipsoid::WGS84.geodeticSurfaceNormal(carto);

        // Create view matrix using lookAt
        // Convert to float for glm
        glm::vec3 posFloat(position.x, position.y, position.z);
        glm::vec3 targetFloat(target.x, target.y, target.z);
        glm::vec3 upFloat(up.x, up.y, up.z);

        m_ViewMatrix = glm::lookAt(posFloat, targetFloat, upFloat);

        // Apply heading and pitch rotation
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), (float)m_Heading, glm::vec3(0, 0, 1)) *
            glm::rotate(glm::mat4(1.0f), (float)(m_Pitch + PTMath::OnePi / 2.0), glm::vec3(1.0f, 0.0f, 0.0f));

        m_ViewMatrix = m_ViewMatrix * rotation;
        m_ViewProjection = m_Projection * m_ViewMatrix;
    }

    void EarthOrbitCamera::RecalculateProjection() {
        m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
        m_ViewProjection = m_Projection * m_ViewMatrix;
    }

}
