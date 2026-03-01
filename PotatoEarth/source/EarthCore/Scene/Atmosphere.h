#pragma once

#include "EarthCore/Core/PotatoEarthBase.h"
#include "EarthCore/Render/Shader.h"
#include <glm/glm.hpp>

namespace PTEarth {

    // Atmospheric scattering parameters
    struct AtmosphereParameters {
        // Rayleigh scattering
        glm::vec3 RayleighScattering = glm::vec3(5.802f, 13.558f, 33.1f) * 1e-6f;
        float RayleighScaleHeight = 8000.0f;  // meters
        
        // Mie scattering
        glm::vec3 MieScattering = glm::vec3(3.996f) * 1e-6f;
        float MieScaleHeight = 1200.0f;  // meters
        float MiePreferredDirection = 0.8f;  // g parameter
        
        // Ground
        float GroundRadius = 6360000.0f;  // Earth radius in meters
        float AtmosphereRadius = 6420000.0f;  // Atmosphere top
        
        // Sun
        glm::vec3 SunDirection = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f));
        float SunIntensity = 20.0f;
    };

    class PTEARTH_API Atmosphere {
    public:
        Atmosphere();
        ~Atmosphere() = default;
        
        void Initialize();
        
        // Render sky dome
        void RenderSky(const glm::mat4& viewProjection, const glm::vec3& cameraPosition);
        
        // Render aerial perspective (fog/atmosphere on objects)
        void RenderAerialPerspective(const glm::mat4& viewProjection);
        
        // Set sun direction
        void SetSunDirection(const glm::vec3& direction) { m_Params.SunDirection = glm::normalize(direction); }
        void SetSunIntensity(float intensity) { m_Params.SunIntensity = intensity; }
        
        // Get parameters for shader
        const AtmosphereParameters& GetParameters() const { return m_Params; }
        
        PT_Ref<Shader> GetSkyShader() { return m_SkyShader; }
        PT_Ref<Shader> GetAerialPerspectiveShader() { return m_AerialShader; }
        
    private:
        void CreateSkyDome();
        void LoadShaders();
        
    private:
        AtmosphereParameters m_Params;
        PT_Ref<Shader> m_SkyShader;
        PT_Ref<Shader> m_AerialShader;
        PT_Ref<VertexArray> m_SkyDomeVAO;
    };

}
