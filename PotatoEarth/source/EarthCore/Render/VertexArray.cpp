#include "potatopch.h"
#include "VertexArray.h"

#include "Platform/Opengl/OpenGLVertexArray.h"

namespace PTEarth {

    PT_Ref<VertexArray> VertexArray::Create() {
        return CreateRef<OpenGLVertexArray>();
    }

}
