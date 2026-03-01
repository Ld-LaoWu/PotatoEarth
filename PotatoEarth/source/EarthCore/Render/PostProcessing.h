#pragma once

#include "EarthCore/Core/PotatoEarthBase.h"
#include "EarthCore/Render/Texture.h"
#include "EarthCore/Render/Shader.h"
#include "EarthCore/Render/Texture.h"
#include <glm/glm.hpp>
#include <vector>
#include <memory>

namespace PTEarth {

    // Post-processing effects
    enum class PostProcessEffect {
        None = 0,
        Bloom = 1,
        SSAO = 2,
        SSR = 4,
        ToneMapping = 8,
        FXAA = 16,
        All = Bloom | SSAO | SSR | ToneMapping | FXAA
    };

    // Framebuffer for post-processing
    class Framebuffer {
    public:
        Framebuffer(uint32_t width, uint32_t height);
        ~Framebuffer();

        void Bind();
        void Unbind();
        void Resize(uint32_t width, uint32_t height);

        uint32_t GetColorAttachment() const { return m_ColorAttachment; }
        uint32_t GetDepthAttachment() const { return m_DepthAttachment; }

    private:
        void Invalidate();

    private:
        uint32_t m_RendererID = 0;
        uint32_t m_ColorAttachment = 0;
        uint32_t m_DepthAttachment = 0;
        uint32_t m_Width, m_Height;
    };

    // Fullscreen quad for post-processing
    class ScreenQuad {
    public:
        ScreenQuad();
        ~ScreenQuad();

        void Render();

    private:
        uint32_t m_VAO = 0;
        uint32_t m_VBO = 0;
    };

    // Post-process stack
    class PostProcessStack {
    public:
        PostProcessStack(uint32_t width, uint32_t height);
        ~PostProcessStack();

        void Initialize();

        // Begin scene rendering to framebuffer
        void BeginScene();
        void EndScene();

        // Apply effects
        void ApplyEffects(uint32_t effects);

        // Individual effects
        void BloomPass(Texture2D& input);
        void SSAOPass(Texture2D& depth, Texture2D& normal);
        void SSRPass(Texture2D& color, Texture2D& depth, Texture2D& normal);
        void ToneMappingPass(Texture2D& input);
        void FXAAPass(Texture2D& input);

        // Settings
        void SetBloomIntensity(float intensity) { m_BloomIntensity = intensity; }
        void SetExposure(float exposure) { m_Exposure = exposure; }
        void SetGamma(float gamma) { m_Gamma = gamma; }

    private:
        void LoadShaders();

    private:
        PT_Ref<Framebuffer> m_SceneFramebuffer;
        PT_Ref<Framebuffer> m_BloomFramebuffer[2];  // Ping-pong buffers
        PT_Ref<Framebuffer> m_SSAOFramebuffer;
        PT_Ref<Framebuffer> m_SSRFramebuffer;
        PT_Ref<Framebuffer> m_FinalFramebuffer;

        PT_Ref<Shader> m_BloomShader;
        PT_Ref<Shader> m_BlurShader;
        PT_Ref<Shader> m_SSAOShader;
        PT_Ref<Shader> m_SSRShader;
        PT_Ref<Shader> m_ToneMappingShader;
        PT_Ref<Shader> m_FXAAShader;

        ScreenQuad m_ScreenQuad;

        uint32_t m_Width, m_Height;

        // Settings
        float m_BloomIntensity = 0.5f;
        float m_Exposure = 1.0f;
        float m_Gamma = 2.2f;
    };

}

