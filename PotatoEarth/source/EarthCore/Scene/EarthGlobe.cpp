#include "potatopch.h"
#include "EarthGlobe.h"

#include "EarthCore/Render/Buffer.h"
#include "EarthCore/Core/Utility/Math/PTMath.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>

namespace PTEarth {

    EarthGlobe::EarthGlobe() {
    }

    void EarthGlobe::Initialize() {
        LoadShaders();
        GenerateSphere(m_Radius, 64, 64);
    }

    void EarthGlobe::LoadShaders() {
        std::string vertexSrc = R"(
            #version 330 core
            
            layout(location = 0) in vec3 a_Position;
            layout(location = 1) in vec3 a_Normal;
            
            uniform mat4 u_ViewProjection;
            uniform mat4 u_Transform;
            
            out vec3 v_Normal;
            out vec3 v_Position;
            
            void main() {
                v_Position = a_Position;
                v_Normal = mat3(transpose(inverse(u_Transform))) * a_Normal;
                gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
            }
        )";

        std::string fragmentSrc = R"(
            #version 330 core
            
            layout(location = 0) out vec4 o_Color;
            
            in vec3 v_Normal;
            in vec3 v_Position;
            
            uniform vec4 u_Color;
            
            void main() {
                // Simple lighting
                vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
                float diff = max(dot(normalize(v_Normal), lightDir), 0.0);
                vec3 ambient = vec3(0.1, 0.1, 0.2);
                vec3 result = ambient + u_Color.rgb * diff;
                o_Color = vec4(result, u_Color.a);
            }
        )";

        m_Shader = Shader::Create("EarthShader", vertexSrc, fragmentSrc);
    }

    void EarthGlobe::GenerateSphere(float radius, uint32_t latSegments, uint32_t lonSegments) {
        std::vector<float> vertices;
        std::vector<uint32_t> indices;

        // Generate vertices
        for (uint32_t lat = 0; lat <= latSegments; ++lat) {
            float theta = (float)lat * PTMath::OnePi / (float)latSegments; // 0 to PI
            float sinTheta = sinf(theta);
            float cosTheta = cosf(theta);

            for (uint32_t lon = 0; lon <= lonSegments; ++lon) {
                float phi = (float)lon * 2.0f * PTMath::OnePi / (float)lonSegments; // 0 to 2PI
                float sinPhi = sinf(phi);
                float cosPhi = cosf(phi);

                // Position
                float x = cosPhi * sinTheta;
                float y = cosTheta;
                float z = sinPhi * sinTheta;

                // Normal (same as position for unit sphere)
                float nx = x;
                float ny = y;
                float nz = z;

                vertices.push_back(x * radius);
                vertices.push_back(y * radius);
                vertices.push_back(z * radius);
                vertices.push_back(nx);
                vertices.push_back(ny);
                vertices.push_back(nz);
            }
        }

        // Generate indices
        for (uint32_t lat = 0; lat < latSegments; ++lat) {
            for (uint32_t lon = 0; lon < lonSegments; ++lon) {
                uint32_t current = lat * (lonSegments + 1) + lon;
                uint32_t next = current + lonSegments + 1;

                indices.push_back(current);
                indices.push_back(next);
                indices.push_back(current + 1);

                indices.push_back(current + 1);
                indices.push_back(next);
                indices.push_back(next + 1);
            }
        }

        // Create buffers
        m_VertexArray = VertexArray::Create();

        PT_Ref<VertexBuffer> vb = VertexBuffer::Create(vertices.data(), (uint32_t)(vertices.size() * sizeof(float)));
        BufferLayout layout = {
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float3, "a_Normal" }
        };
        vb->SetLayout(layout);

        PT_Ref<IndexBuffer> ib = IndexBuffer::Create(indices.data(), (uint32_t)indices.size());

        m_VertexArray->AddVertexBuffer(vb);
        m_VertexArray->SetIndexBuffer(ib);

        PT_EARTH_INFO("Earth sphere generated: {0} vertices, {1} indices", vertices.size() / 6, indices.size());
    }

    void EarthGlobe::Render(const glm::mat4& viewProjection, const glm::mat4& transform) {
        if (m_Shader) {
            m_Shader->Bind();
            m_Shader->SetMat4("u_ViewProjection", viewProjection);
            m_Shader->SetMat4("u_Transform", transform);
            m_Shader->SetFloat4("u_Color", glm::vec4(0.2f, 0.4f, 0.8f, 1.0f)); // Blue earth
        }

        if (m_VertexArray) {
            m_VertexArray->Bind();
            glDrawElements(GL_TRIANGLES, m_VertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
        }
    }

}
