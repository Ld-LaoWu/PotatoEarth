#pragma once

#include "EarthCore/Core/PotatoEarthBase.h"
#include "EarthCore/Render/Shader.h"
#include "EarthCore/Render/Texture.h"
#include <glm/glm.hpp>

namespace PTEarth {

    // PBR Material properties
    struct PBRMaterial {
        glm::vec3 Albedo = glm::vec3(0.5f, 0.5f, 0.5f);
        float Metallic = 0.0f;
        float Roughness = 0.5f;
        float AO = 1.0f;
        glm::vec3 Emissive = glm::vec3(0.0f);
        
        // Textures
        PT_Ref<Texture> AlbedoMap;
        PT_Ref<Texture> NormalMap;
        PT_Ref<Texture> MetallicMap;
        PT_Ref<Texture> RoughnessMap;
        PT_Ref<Texture> AOMap;
        PT_Ref<Texture> EmissiveMap;
        
        bool UseAlbedoMap = false;
        bool UseNormalMap = false;
        bool UseMetallicMap = false;
        bool UseRoughnessMap = false;
        bool UseAOMap = false;
        bool UseEmissiveMap = false;
    };

    class PTEARTH_API Material {
    public:
        Material(const std::string& name);
        ~Material() = default;
        
        void Bind(const PT_Ref<Shader>& shader);
        
        // Setters
        void SetAlbedo(const glm::vec3& albedo) { m_Material.Albedo = albedo; }
        void SetMetallic(float metallic) { m_Material.Metallic = metallic; }
        void SetRoughness(float roughness) { m_Material.Roughness = roughness; }
        void SetAO(float ao) { m_Material.AO = ao; }
        void SetEmissive(const glm::vec3& emissive) { m_Material.Emissive = emissive; }
        
        void SetAlbedoMap(const PT_Ref<Texture>& texture) { m_Material.AlbedoMap = texture; m_Material.UseAlbedoMap = (texture != nullptr); }
        void SetNormalMap(const PT_Ref<Texture>& texture) { m_Material.NormalMap = texture; m_Material.UseNormalMap = (texture != nullptr); }
        void SetMetallicMap(const PT_Ref<Texture>& texture) { m_Material.MetallicMap = texture; m_Material.UseMetallicMap = (texture != nullptr); }
        void SetRoughnessMap(const PT_Ref<Texture>& texture) { m_Material.RoughnessMap = texture; m_Material.UseRoughnessMap = (texture != nullptr); }
        void SetAOMap(const PT_Ref<Texture>& texture) { m_Material.AOMap = texture; m_Material.UseAOMap = (texture != nullptr); }
        void SetEmissiveMap(const PT_Ref<Texture>& texture) { m_Material.EmissiveMap = texture; m_Material.UseEmissiveMap = (texture != nullptr); }
        
        const std::string& GetName() const { return m_Name; }
        const PBRMaterial& GetProperties() const { return m_Material; }
        
        // Create default materials
        static PT_Ref<Material> CreateDefault();
        static PT_Ref<Material> CreateEarth();
        static PT_Ref<Material> CreateMetal();
        static PT_Ref<Material> CreatePlastic();
        
    private:
        std::string m_Name;
        PBRMaterial m_Material;
    };

}
