#include "potatopch.h"
#include "Atmosphere.h"

#include "EarthCore/Render/Buffer.h"
#include "EarthCore/Render/VertexArray.h"
#include <glad/glad.h>
#include <math.h>

namespace PTEarth {

    Atmosphere::Atmosphere() {
    }

    void Atmosphere::Initialize() {
        LoadShaders();
        CreateSkyDome();
        PT_EARTH_INFO("Atmosphere initialized");
    }

    void Atmosphere::LoadShaders() {
        // Sky dome vertex shader
        std::string skyVertexShader = R"(
            #version 330 core
            layout(location = 0) in vec3 a_Position;
            
            uniform mat4 u_ViewProjection;
            uniform mat4 u_Model;
            
            out vec3 v_Position;
            
            void main() {
                v_Position = a_Position;
                gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
            }
        )";

        // Sky dome fragment shader - Simplified atmospheric scattering
        std::string skyFragmentShader = R"(
            #version 330 core
            layout(location = 0) out vec4 o_Color;
            
            in vec3 v_Position;
            
            uniform vec3 u_SunDirection;
            uniform float u_SunIntensity;
            uniform vec3 u_RayleighScattering;
            uniform float u_RayleighScaleHeight;
            uniform vec3 u_MieScattering;
            uniform float u_MieScaleHeight;
            uniform float u_MieG;
            uniform vec3 u_CameraPosition;
            uniform float u_GroundRadius;
            uniform float u_AtmosphereRadius;
            
            const float PI = 3.14159265359;
            
            // Rayleigh phase function
            float RayleighPhase(float cosTheta) {
                return (3.0 / (16.0 * PI)) * (1.0 + cosTheta * cosTheta);
            }
            
            // Mie phase function (Henyey-Greenstein)
            float MiePhase(float cosTheta, float g) {
                float g2 = g * g;
                return (1.0 / (4.0 * PI)) * ((1.0 - g2) / pow(1.0 + g2 - 2.0 * g * cosTheta, 1.5));
            }
            
            void main() {
                vec3 viewDir = normalize(v_Position - u_CameraPosition);
                float cosTheta = dot(viewDir, u_SunDirection);
                
                // Phase functions
                float rayleighPhase = RayleighPhase(cosTheta);
                float miePhase = MiePhase(cosTheta, u_MieG);
                
                // Simplified scattering approximation
                vec3 rayleigh = u_RayleighScattering * rayleighPhase;
                vec3 mie = u_MieScattering * miePhase;
                
                // Horizon effect
                float horizon = max(0.0, viewDir.y);
                vec3 skyColor = (rayleigh + mie) * u_SunIntensity;
                
                // Ground fade
                float groundFactor = smoothstep(-0.1, 0.0, viewDir.y);
                
                // Sun disc
                float sunAngle = acos(cosTheta);
                float sunDisc = smoothstep(0.02, 0.0, sunAngle);
                vec3 sunColor = vec3(1.0, 0.9, 0.7) * sunDisc * u_SunIntensity;
                
                vec3 finalColor = skyColor * groundFactor + sunColor;
                
                o_Color = vec4(finalColor, 1.0);
            }
        )";

        m_SkyShader = Shader::Create("SkyShader", skyVertexShader, skyFragmentShader);

        // Aerial perspective shader (simplified)
        std::string aerialVertexShader = R"(
            #version 330 core
            layout(location = 0) in vec3 a_Position;
            layout(location = 1) in vec3 a_Normal;
            
            uniform mat4 u_ViewProjection;
            uniform mat4 u_Model;
            
            out vec3 v_Position;
            out vec3 v_Normal;
            
            void main() {
                v_Position = (u_Model * vec4(a_Position, 1.0)).xyz;
                v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;
                gl_Position = u_ViewProjection * vec4(v_Position, 1.0);
            }
        )";

        std::string aerialFragmentShader = R"(
            #version 330 core
            layout(location = 0) out vec4 o_Color;
            
            in vec3 v_Position;
            in vec3 v_Normal;
            
            uniform vec3 u_SunDirection;
            uniform float u_SunIntensity;
            uniform vec3 u_RayleighScattering;
            uniform vec3 u_MieScattering;
            uniform vec3 u_CameraPosition;
            uniform float u_AtmosphereRadius;
            
            void main() {
                vec3 viewDir = normalize(u_CameraPosition - v_Position);
                float cosTheta = dot(viewDir, u_SunDirection);
                
                // Distance from camera
                float distance = length(v_Position - u_CameraPosition);
                
                // Simplified aerial perspective
                float opticalDepth = distance / u_AtmosphereRadius;
                vec3 extinction = exp(-(u_RayleighScattering + u_MieScattering) * opticalDepth);
                
                // In-scattering
                vec3 scattering = (u_RayleighScattering + u_MieScattering) * u_SunIntensity * max(0.0, dot(v_Normal, u_SunDirection));
                
                vec3 finalColor = scattering * (1.0 - extinction) + extinction;
                
                o_Color = vec4(finalColor, 1.0);
            }
        )";

        m_AerialShader = Shader::Create("AerialShader", aerialVertexShader, aerialFragmentShader);
    }

    void Atmosphere::CreateSkyDome() {
        // Create a sphere for sky dome
        std::vector<float> vertices;
        std::vector<uint32_t> indices;
        
        const uint32_t stacks = 16;
        const uint32_t slices = 32;
        const float radius = m_Params.AtmosphereRadius / 1000.0f;  // Scale down for rendering
        
        for (uint32_t stack = 0; stack <= stacks; ++stack) {
            float phi = PI * float(stack) / float(stacks);
            float sinPhi = sinf(phi);
            float cosPhi = cosf(phi);
            
            for (uint32_t slice = 0; slice <= slices; ++slice) {
                float theta = 2.0f * PI * float(slice) / float(slices);
                float sinTheta = sinf(theta);
                float cosTheta = cosf(theta);
                
                float x = cosTheta * sinPhi * radius;
                float y = cosPhi * radius;
                float z = sinTheta * sinPhi * radius;
                
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);
            }
        }
        
        for (uint32_t stack = 0; stack < stacks; ++stack) {
            for (uint32_t slice = 0; slice < slices; ++slice) {
                uint32_t i0 = stack * (slices + 1) + slice;
                uint32_t i1 = stack * (slices + 1) + slice + 1;
                uint32_t i2 = (stack + 1) * (slices + 1) + slice;
                uint32_t i3 = (stack + 1) * (slices + 1) + slice + 1;
                
                indices.push_back(i0);
                indices.push_back(i2);
                indices.push_back(i1);
                
                indices.push_back(i1);
                indices.push_back(i2);
                indices.push_back(i3);
            }
        }
        
        m_SkyDomeVAO = VertexArray::Create();
        
        PT_Ref<VertexBuffer> vb = VertexBuffer::Create(vertices.data(), vertices.size() * sizeof(float));
        BufferLayout layout = {
            { ShaderDataType::Float3, "a_Position" }
        };
        vb->SetLayout(layout);
        
        PT_Ref<IndexBuffer> ib = IndexBuffer::Create(indices.data(), indices.size());
        
        m_SkyDomeVAO->AddVertexBuffer(vb);
        m_SkyDomeVAO->SetIndexBuffer(ib);
    }

    void Atmosphere::RenderSky(const glm::mat4& viewProjection, const glm::vec3& cameraPosition) {
        if (!m_SkyShader || !m_SkyDomeVAO) return;
        
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        
        m_SkyShader->Bind();
        
        // Set uniforms
        m_SkyShader->SetMat4("u_ViewProjection", viewProjection);
        m_SkyShader->SetMat4("u_Model", glm::mat4(1.0f));
        m_SkyShader->SetFloat3("u_SunDirection", m_Params.SunDirection);
        m_SkyShader->SetFloat("u_SunIntensity", m_Params.SunIntensity);
        m_SkyShader->SetFloat3("u_RayleighScattering", m_Params.RayleighScattering);
        m_SkyShader->SetFloat("u_RayleighScaleHeight", m_Params.RayleighScaleHeight);
        m_SkyShader->SetFloat3("u_MieScattering", m_Params.MieScattering);
        m_SkyShader->SetFloat("u_MieScaleHeight", m_Params.MieScaleHeight);
        m_SkyShader->SetFloat("u_MieG", m_Params.MiePreferredDirection);
        m_SkyShader->SetFloat3("u_CameraPosition", cameraPosition);
        m_SkyShader->SetFloat("u_GroundRadius", m_Params.GroundRadius);
        m_SkyShader->SetFloat("u_AtmosphereRadius", m_Params.AtmosphereRadius);
        
        m_SkyDomeVAO->Bind();
        glDrawElements(GL_TRIANGLES, m_SkyDomeVAO->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
        
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
    }

    void Atmosphere::RenderAerialPerspective(const glm::mat4& viewProjection) {
        // This would be called for each object to apply atmospheric fog
        // For now, this is a placeholder
    }

}
