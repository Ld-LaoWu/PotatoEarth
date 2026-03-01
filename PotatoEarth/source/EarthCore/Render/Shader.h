#pragma once

#include "EarthCore/Core/PotatoEarthBase.h"
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

namespace PTEarth {

    class PTEARTH_API Shader {
    public:
        virtual ~Shader() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void SetInt(const std::string& name, int value) = 0;
        virtual void SetFloat(const std::string& name, float value) = 0;
        virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
        virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
        virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

        virtual const std::string& GetName() const = 0;

        static PT_Ref<Shader> Create(const std::string& filepath);
        static PT_Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
    };

    class PTEARTH_API ShaderLibrary {
    public:
        void Add(const std::string& name, const PT_Ref<Shader>& shader);
        PT_Ref<Shader> Get(const std::string& name);
        bool Exists(const std::string& name) const;
    private:
        std::unordered_map<std::string, PT_Ref<Shader>> m_Shaders;
    };

}
