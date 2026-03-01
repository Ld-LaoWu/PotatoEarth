#pragma once

#include "EarthCore/Render/VertexArray.h"

namespace PTEarth {

    class OpenGLVertexArray : public VertexArray {
    public:
        OpenGLVertexArray();
        virtual ~OpenGLVertexArray();

        void Bind() const override;
        void Unbind() const override;

        void AddVertexBuffer(const PT_Ref<VertexBuffer>& vertexBuffer) override;
        void SetIndexBuffer(const PT_Ref<IndexBuffer>& indexBuffer) override;

        const std::vector<PT_Ref<VertexBuffer>>& GetVertexBuffers() const override { return m_VertexBuffers; }
        const PT_Ref<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; }

    private:
        uint32_t m_RendererID;
        uint32_t m_VertexBufferIndex = 0;
        std::vector<PT_Ref<VertexBuffer>> m_VertexBuffers;
        PT_Ref<IndexBuffer> m_IndexBuffer;
    };

}
