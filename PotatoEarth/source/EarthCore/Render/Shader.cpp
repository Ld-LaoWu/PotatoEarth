#include "potatopch.h"
#include "Shader.h"

#include "Platform/Opengl/OpenGLShader.h"

namespace PTEarth {

    PT_Ref<Shader> Shader::Create(const std::string& filepath) {
        return CreateRef<OpenGLShader>(filepath);
    }

    PT_Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc) {
        return CreateRef<OpenGLShader>(name, vertexSrc, fragmentSrc);
    }

    // ShaderLibrary //
    void ShaderLibrary::Add(const std::string& name, const PT_Ref<Shader>& shader) {
        m_Shaders[name] = shader;
    }

    PT_Ref<Shader> ShaderLibrary::Get(const std::string& name) {
        if (Exists(name)) {
            return m_Shaders[name];
        }
        return nullptr;
    }

    bool ShaderLibrary::Exists(const std::string& name) const {
        return m_Shaders.find(name) != m_Shaders.end();
    }

}
