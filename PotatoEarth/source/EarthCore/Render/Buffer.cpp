#include "potatopch.h"
#include "Buffer.h"

#include "Platform/Opengl/OpenGLBuffer.h"

namespace PTEarth {

    PT_Ref<VertexBuffer> VertexBuffer::Create(uint32_t size) {
        return CreateRef<OpenGLVertexBuffer>(size);
    }

    PT_Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size) {
        return CreateRef<OpenGLVertexBuffer>(vertices, size);
    }

    PT_Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count) {
        return CreateRef<OpenGLIndexBuffer>(indices, count);
    }

}
