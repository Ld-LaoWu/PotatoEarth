#pragma once

#include "EarthCore/Core/PotatoEarthBase.h"
#include "EarthCore/Render/Buffer.h"

namespace PTEarth {

    class VertexArray {
    public:
        virtual ~VertexArray() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void AddVertexBuffer(const PT_Ref<VertexBuffer>& vertexBuffer) = 0;
        virtual void SetIndexBuffer(const PT_Ref<IndexBuffer>& indexBuffer) = 0;

        virtual const std::vector<PT_Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
        virtual const PT_Ref<IndexBuffer>& GetIndexBuffer() const = 0;

        static PT_Ref<VertexArray> Create();
    };

}
