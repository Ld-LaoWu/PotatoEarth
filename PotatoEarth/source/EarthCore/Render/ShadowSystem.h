#pragma once

#include "EarthCore/Core/PotatoEarthBase.h"
#include "EarthCore/Render/Framebuffer.h"
#include "EarthCore/Render/Shader.h"
#include <glm/glm.hpp>

namespace PTEarth {

    // Shadow map configuration
    struct ShadowConfig {
        uint32_t Resolution = 2048;
        float NearPlane = 1.0f;
        float FarPlane = 1000.0f;
        glm::vec3 LightDirection = glm::normalize(glm::vec3(1.0f, -1.0f, 0.5f));
        float OrthoSize = 50.0f;  // For directional light
        bool SoftShadows = true;
        uint32_t PCFSamples = 16;  // Percentage-closer filtering
    };

    class ShadowMap {
    public:
        ShadowMap(const ShadowConfig& config);
        ~ShadowMap();

        void Initialize();

        // Begin/end shadow pass
        void BeginPass();
        void EndPass();

        // Get matrices
        glm::mat4 GetLightSpaceMatrix() const { return m_LightSpaceMatrix; }
        uint32_t GetShadowMapTexture() const { return m_DepthTexture; }
        const ShadowConfig& GetConfig() const { return m_Config; }

        // Update light direction
        void SetLightDirection(const glm::vec3& direction) { 
            m_Config.LightDirection = glm::normalize(direction); 
            UpdateMatrices();
        }

    private:
        void UpdateMatrices();

    private:
        ShadowConfig m_Config;
        
        uint32_t m_FBO = 0;
        uint32_t m_DepthTexture = 0;
        
        glm::mat4 m_LightProjection;
        glm::mat4 m_LightView;
        glm::mat4 m_LightSpaceMatrix;
    };

    // Cascaded Shadow Maps for large scenes
    struct Cascade {
        float SplitDistance;
        glm::mat4 LightSpaceMatrix;
        PT_Ref<Framebuffer> ShadowFramebuffer;
    };

    class CascadedShadowMap {
    public:
        static const int CASCADE_COUNT = 4;

        CascadedShadowMap(uint32_t resolution = 2048);
        ~CascadedShadowMap();

        void Initialize();

        // Calculate cascade splits based on camera
        void UpdateCascades(const Camera& camera, const glm::vec3& lightDirection);

        // Render each cascade
        void BeginCascade(int index);
        void EndCascade(int index);

        // Get data
        const Cascade& GetCascade(int index) const { return m_Cascades[index]; }
        const std::vector<float>& GetCascadeSplits() const { return m_CascadeSplits; }

    private:
        void CalculateSplits(const Camera& camera);
        glm::mat4 GetLightSpaceMatrix(const Camera& camera, float nearPlane, float farPlane, const glm::vec3& lightDir);

    private:
        uint32_t m_Resolution;
        std::array<Cascade, CASCADE_COUNT> m_Cascades;
        std::vector<float> m_CascadeSplits;
        glm::vec3 m_LightDirection;
    };

    // Shadow system manager
    class ShadowSystem {
    public:
        void Initialize();
        void Shutdown();

        // Set shadow map type
        void UseSimpleShadowMap(const ShadowConfig& config);
        void UseCascadedShadowMap(uint32_t resolution);

        // Update and render shadows
        void Update(const Camera& camera, const glm::vec3& lightDir);
        void BeginShadowPass();
        void EndShadowPass();

        // Get shadow data for main shader
        uint32_t GetShadowMapTexture() const;
        glm::mat4 GetLightSpaceMatrix() const;
        bool UseCascades() const { return m_UseCascades; }

    private:
        PT_Ref<ShadowMap> m_SimpleShadowMap;
        PT_Ref<CascadedShadowMap> m_CascadedShadowMap;
        bool m_UseCascades = false;
    };

}
