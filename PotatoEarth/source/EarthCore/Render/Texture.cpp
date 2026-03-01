#include "potatopch.h"
#include "Texture.h"

#include "Platform/Opengl/OpenGLTexture.h"

namespace PTEarth {

    PT_Ref<Texture> Texture::Create(const std::string& path) {
        return CreateRef<OpenGLTexture>(path);
    }

    PT_Ref<Texture> Texture::Create(uint32_t width, uint32_t height) {
        return CreateRef<OpenGLTexture>(width, height);
    }

}
