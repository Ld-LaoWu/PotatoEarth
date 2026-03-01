#pragma once

#include "EarthCore/Core/PotatoEarthBase.h"
#include "EarthCore/Render/VertexArray.h"
#include "EarthCore/Render/Shader.h"

namespace PTEarth {

    class EarthGlobe {
    public:
        EarthGlobe();
        ~EarthGlobe() = default;

        void Initialize();
        void Render(const glm::mat4& viewProjection, const glm::mat4& transform);

        PT_Ref<VertexArray> GetVertexArray() const { return m_VertexArray; }
        PT_Ref<Shader> GetShader() const { return m_Shader; }

    private:
        void GenerateSphere(float radius, uint32_t latSegments, uint32_t lonSegments);
        void LoadShaders();

    private:
        PT_Ref<VertexArray> m_VertexArray;
        PT_Ref<Shader> m_Shader;
        float m_Radius = 6371000.0f; // Earth radius in meters
    };

}
