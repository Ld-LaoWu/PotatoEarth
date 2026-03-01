#include "potatopch.h"
#include "Material.h"

namespace PTEarth {

    Material::Material(const std::string& name)
        : m_Name(name) {
    }

    void Material::Bind(const PT_Ref<Shader>& shader) {
        if (!shader) return;
        
        // Bind material properties
        shader->SetFloat3("u_Material.Albedo", m_Material.Albedo);
        shader->SetFloat("u_Material.Metallic", m_Material.Metallic);
        shader->SetFloat("u_Material.Roughness", m_Material.Roughness);
        shader->SetFloat("u_Material.AO", m_Material.AO);
        shader->SetFloat3("u_Material.Emissive", m_Material.Emissive);
        
        // Bind textures
        int textureSlot = 0;
        
        if (m_Material.UseAlbedoMap && m_Material.AlbedoMap) {
            m_Material.AlbedoMap->Bind(textureSlot);
            shader->SetInt("u_AlbedoMap", textureSlot++);
        }
        
        if (m_Material.UseNormalMap && m_Material.NormalMap) {
            m_Material.NormalMap->Bind(textureSlot);
            shader->SetInt("u_NormalMap", textureSlot++);
        }
        
        if (m_Material.UseMetallicMap && m_Material.MetallicMap) {
            m_Material.MetallicMap->Bind(textureSlot);
            shader->SetInt("u_MetallicMap", textureSlot++);
        }
        
        if (m_Material.UseRoughnessMap && m_Material.RoughnessMap) {
            m_Material.RoughnessMap->Bind(textureSlot);
            shader->SetInt("u_RoughnessMap", textureSlot++);
        }
        
        if (m_Material.UseAOMap && m_Material.AOMap) {
            m_Material.AOMap->Bind(textureSlot);
            shader->SetInt("u_AOMap", textureSlot++);
        }
        
        if (m_Material.UseEmissiveMap && m_Material.EmissiveMap) {
            m_Material.EmissiveMap->Bind(textureSlot);
            shader->SetInt("u_EmissiveMap", textureSlot++);
        }
        
        // Set texture usage flags
        shader->SetInt("u_UseAlbedoMap", m_Material.UseAlbedoMap ? 1 : 0);
        shader->SetInt("u_UseNormalMap", m_Material.UseNormalMap ? 1 : 0);
        shader->SetInt("u_UseMetallicMap", m_Material.UseMetallicMap ? 1 : 0);
        shader->SetInt("u_UseRoughnessMap", m_Material.UseRoughnessMap ? 1 : 0);
        shader->SetInt("u_UseAOMap", m_Material.UseAOMap ? 1 : 0);
        shader->SetInt("u_UseEmissiveMap", m_Material.UseEmissiveMap ? 1 : 0);
    }

    PT_Ref<Material> Material::CreateDefault() {
        auto mat = CreateRef<Material>("Default");
        mat->SetAlbedo(glm::vec3(0.5f, 0.5f, 0.5f));
        mat->SetMetallic(0.0f);
        mat->SetRoughness(0.5f);
        mat->SetAO(1.0f);
        return mat;
    }

    PT_Ref<Material> Material::CreateEarth() {
        auto mat = CreateRef<Material>("Earth");
        mat->SetAlbedo(glm::vec3(0.1f, 0.3f, 0.6f));  // Ocean blue
        mat->SetMetallic(0.0f);
        mat->SetRoughness(0.8f);  // Rough surface
        mat->SetAO(1.0f);
        return mat;
    }

    PT_Ref<Material> Material::CreateMetal() {
        auto mat = CreateRef<Material>("Metal");
        mat->SetAlbedo(glm::vec3(0.8f, 0.8f, 0.8f));
        mat->SetMetallic(1.0f);
        mat->SetRoughness(0.2f);
        mat->SetAO(1.0f);
        return mat;
    }

    PT_Ref<Material> Material::CreatePlastic() {
        auto mat = CreateRef<Material>("Plastic");
        mat->SetAlbedo(glm::vec3(0.9f, 0.1f, 0.1f));
        mat->SetMetallic(0.0f);
        mat->SetRoughness(0.1f);  // Smooth
        mat->SetAO(1.0f);
        return mat;
    }

}
