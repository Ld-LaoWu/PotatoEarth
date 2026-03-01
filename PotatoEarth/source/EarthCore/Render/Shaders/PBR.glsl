// PBR Vertex Shader
const char* PBRVertexShader = R"(
    #version 330 core
    
    layout(location = 0) in vec3 a_Position;
    layout(location = 1) in vec3 a_Normal;
    layout(location = 2) in vec2 a_TexCoord;
    
    uniform mat4 u_ViewProjection;
    uniform mat4 u_Model;
    uniform mat4 u_NormalMatrix;
    
    out vec3 v_Position;
    out vec3 v_Normal;
    out vec2 v_TexCoord;
    
    void main() {
        vec4 worldPos = u_Model * vec4(a_Position, 1.0);
        v_Position = worldPos.xyz;
        v_Normal = mat3(u_NormalMatrix) * a_Normal;
        v_TexCoord = a_TexCoord;
        gl_Position = u_ViewProjection * worldPos;
    }
)";

// PBR Fragment Shader
const char* PBRFragmentShader = R"(
    #version 330 core
    
    layout(location = 0) out vec4 o_Color;
    
    in vec3 v_Position;
    in vec3 v_Normal;
    in vec2 v_TexCoord;
    
    // Material
    struct Material {
        vec3 Albedo;
        float Metallic;
        float Roughness;
        float AO;
        vec3 Emissive;
    };
    uniform Material u_Material;
    
    // Material textures
    uniform sampler2D u_AlbedoMap;
    uniform sampler2D u_NormalMap;
    uniform sampler2D u_MetallicMap;
    uniform sampler2D u_RoughnessMap;
    uniform sampler2D u_AOMap;
    uniform sampler2D u_EmissiveMap;
    
    uniform int u_UseAlbedoMap;
    uniform int u_UseNormalMap;
    uniform int u_UseMetallicMap;
    uniform int u_UseRoughnessMap;
    uniform int u_UseAOMap;
    uniform int u_UseEmissiveMap;
    
    // Lighting
    uniform vec3 u_SunDirection;
    uniform vec3 u_SunColor;
    uniform float u_SunIntensity;
    uniform vec3 u_CameraPosition;
    
    const float PI = 3.14159265359;
    
    // Normal distribution function (GGX/Trowbridge-Reitz)
    float DistributionGGX(vec3 N, vec3 H, float roughness) {
        float a = roughness * roughness;
        float a2 = a * a;
        float NdotH = max(dot(N, H), 0.0);
        float NdotH2 = NdotH * NdotH;
        
        float denom = (NdotH2 * (a2 - 1.0) + 1.0);
        denom = PI * denom * denom;
        
        return a2 / denom;
    }
    
    // Geometry function (Schlick-GGX)
    float GeometrySchlickGGX(float NdotV, float roughness) {
        float r = (roughness + 1.0);
        float k = (r * r) / 8.0;
        
        return NdotV / (NdotV * (1.0 - k) + k);
    }
    
    // Geometry function (Smith's method)
    float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
        float NdotV = max(dot(N, V), 0.0);
        float NdotL = max(dot(N, L), 0.0);
        float ggx1 = GeometrySchlickGGX(NdotV, roughness);
        float ggx2 = GeometrySchlickGGX(NdotL, roughness);
        
        return ggx1 * ggx2;
    }
    
    // Fresnel function (Schlick approximation)
    vec3 FresnelSchlick(float cosTheta, vec3 F0) {
        return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
    }
    
    void main() {
        // Sample material properties
        vec3 albedo = u_Material.Albedo;
        if (u_UseAlbedoMap == 1) {
            albedo = texture(u_AlbedoMap, v_TexCoord).rgb;
        }
        
        float metallic = u_Material.Metallic;
        if (u_UseMetallicMap == 1) {
            metallic = texture(u_MetallicMap, v_TexCoord).r;
        }
        
        float roughness = u_Material.Roughness;
        if (u_UseRoughnessMap == 1) {
            roughness = texture(u_RoughnessMap, v_TexCoord).r;
        }
        
        float ao = u_Material.AO;
        if (u_UseAOMap == 1) {
            ao = texture(u_AOMap, v_TexCoord).r;
        }
        
        vec3 emissive = u_Material.Emissive;
        if (u_UseEmissiveMap == 1) {
            emissive = texture(u_EmissiveMap, v_TexCoord).rgb;
        }
        
        // Normalize inputs
        vec3 N = normalize(v_Normal);
        if (u_UseNormalMap == 1) {
            // Normal mapping would go here
        }
        
        vec3 V = normalize(u_CameraPosition - v_Position);
        vec3 L = normalize(-u_SunDirection);
        vec3 H = normalize(V + L);
        
        // Reflectance at normal incidence
        vec3 F0 = vec3(0.04);
        F0 = mix(F0, albedo, metallic);
        
        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);
        
        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
        vec3 specular = numerator / denominator;
        
        // Energy conservation
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;
        
        // Lambertian diffuse
        float NdotL = max(dot(N, L), 0.0);
        vec3 diffuse = kD * albedo / PI;
        
        // Directional light
        vec3 radiance = u_SunColor * u_SunIntensity;
        vec3 Lo = (diffuse + specular) * radiance * NdotL;
        
        // Ambient
        vec3 ambient = vec3(0.03) * albedo * ao;
        
        // Final color
        vec3 color = ambient + Lo + emissive;
        
        // HDR tonemapping
        color = color / (color + vec3(1.0));
        
        // Gamma correction
        color = pow(color, vec3(1.0 / 2.2));
        
        o_Color = vec4(color, 1.0);
    }
)";
