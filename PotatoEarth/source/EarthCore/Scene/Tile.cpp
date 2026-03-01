#include "potatopch.h"
#include "Tile.h"
#include "EarthCore/Render/Buffer.h"
#include "EarthCore/Core/Geospatial/Ellipsoid.h"
#include "EarthCore/Core/Utility/Math/PTMath.h"
#include <glad/glad.h>
#include <math.h>

namespace PTEarth {

    Tile::Tile(const TileID& id)
        : m_ID(id), m_State(TileState::Unloaded) {
        CalculateBounds();
    }

    Tile::~Tile() {
    }

    void Tile::Initialize() {
        GenerateMesh();
        m_State = TileState::Loaded;
    }

    void Tile::CalculateBounds() {
        int n = 1 << m_ID.Level;

        double lonWest = (m_ID.X / (double)n) * 2.0 * PTMath::OnePi - PTMath::OnePi;
        double lonEast = ((m_ID.X + 1) / (double)n) * 2.0 * PTMath::OnePi - PTMath::OnePi;

        double mercatorY_n = PTMath::OnePi * (1 - 2 * m_ID.Y / (double)n);
        double mercatorY_s = PTMath::OnePi * (1 - 2 * (m_ID.Y + 1) / (double)n);
        
        double latNorth = atan(sinh(mercatorY_n));
        double latSouth = atan(sinh(mercatorY_s));

        m_Southwest = Cartographic(lonWest, latSouth, 0.0);
        m_Northeast = Cartographic(lonEast, latNorth, 0.0);
    }

    void Tile::GenerateMesh() {
        std::vector<float> vertices;
        std::vector<uint32_t> indices;

        const uint32_t segments = 8;

        for (uint32_t y = 0; y <= segments; ++y) {
            double latFrac = y / (double)segments;
            double lat = m_Southwest.latitude * (1.0 - latFrac) + m_Northeast.latitude * latFrac;

            for (uint32_t x = 0; x <= segments; ++x) {
                double lonFrac = x / (double)segments;
                double lon = m_Southwest.longitude * (1.0 - lonFrac) + m_Northeast.longitude * lonFrac;

                Cartographic carto(lon, lat, 0.0);
                glm::dvec3 pos = Ellipsoid::WGS84.cartographicToCartesian(carto);
                glm::dvec3 normal = Ellipsoid::WGS84.geodeticSurfaceNormal(carto);

                float u = (float)lonFrac;
                float v = (float)latFrac;

                vertices.push_back((float)pos.x);
                vertices.push_back((float)pos.y);
                vertices.push_back((float)pos.z);
                vertices.push_back((float)normal.x);
                vertices.push_back((float)normal.y);
                vertices.push_back((float)normal.z);
                vertices.push_back(u);
                vertices.push_back(v);
            }
        }

        for (uint32_t y = 0; y < segments; ++y) {
            for (uint32_t x = 0; x < segments; ++x) {
                uint32_t current = y * (segments + 1) + x;
                uint32_t next = current + segments + 1;

                indices.push_back(current);
                indices.push_back(next);
                indices.push_back(current + 1);

                indices.push_back(current + 1);
                indices.push_back(next);
                indices.push_back(next + 1);
            }
        }

        m_VertexArray = VertexArray::Create();

        PT_Ref<VertexBuffer> vb = VertexBuffer::Create(vertices.data(), (uint32_t)(vertices.size() * sizeof(float)));
        BufferLayout layout = {
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float3, "a_Normal" },
            { ShaderDataType::Float2, "a_TexCoord" }
        };
        vb->SetLayout(layout);

        PT_Ref<IndexBuffer> ib = IndexBuffer::Create(indices.data(), (uint32_t)indices.size());

        m_VertexArray->AddVertexBuffer(vb);
        m_VertexArray->SetIndexBuffer(ib);
    }

    void Tile::Render(const glm::mat4& viewProjection, const PT_Ref<Shader>& shader) {
        if (m_State != TileState::Loaded || !m_VertexArray) return;

        if (shader) {
            shader->Bind();
            shader->SetMat4("u_ViewProjection", viewProjection);
            shader->SetMat4("u_Transform", glm::mat4(1.0f));
        }

        m_VertexArray->Bind();
        glDrawElements(GL_TRIANGLES, m_VertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
    }

    TileID Tile::GetChild(int index) const {
        int childX = m_ID.X * 2 + (index % 2);
        int childY = m_ID.Y * 2 + (index / 2);
        return TileID(m_ID.Level + 1, childX, childY);
    }

}