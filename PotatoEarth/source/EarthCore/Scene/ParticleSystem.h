#pragma once

#include "EarthCore/Core/PotatoEarthBase.h"
#include "EarthCore/Render/Shader.h"
#include "EarthCore/Render/Buffer.h"
#include <glm/glm.hpp>
#include <vector>
#include <math.h>

namespace PTEarth {

    // Particle data
    struct Particle {
        glm::vec3 Position;
        glm::vec3 Velocity;
        glm::vec4 Color;
        float Size;
        float Life;
        float MaxLife;
        float Rotation;
        float RotationSpeed;
    };

    // Emitter shape
    enum class EmitterShape {
        Point,
        Sphere,
        Box,
        Cone,
        Circle
    };

    // Particle system configuration
    struct ParticleSystemConfig {
        // Emission
        uint32_t MaxParticles = 1000;
        float EmissionRate = 100.0f;  // particles per second
        EmitterShape Shape = EmitterShape::Sphere;
        float ShapeRadius = 1.0f;
        glm::vec3 ShapeExtents = glm::vec3(1.0f);
        
        // Velocity
        float StartSpeed = 5.0f;
        float SpeedVariation = 0.0f;
        glm::vec3 VelocityDirection = glm::vec3(0.0f, 1.0f, 0.0f);
        float VelocitySpread = 0.5f;  // 0-1
        
        // Life
        float StartLife = 2.0f;
        float LifeVariation = 0.5f;
        
        // Size
        float StartSize = 1.0f;
        float EndSize = 0.0f;
        float SizeVariation = 0.0f;
        
        // Color
        glm::vec4 StartColor = glm::vec4(1.0f);
        glm::vec4 EndColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
        
        // Rotation
        float StartRotation = 0.0f;
        float RotationSpeed = 0.0f;
        
        // Physics
        glm::vec3 Gravity = glm::vec3(0.0f, -9.8f, 0.0f);
        float Drag = 0.0f;
        
        // Texture
        PT_Ref<Texture> ParticleTexture;
        bool UseTexture = false;
    };

    class ParticleSystem {
    public:
        ParticleSystem(const ParticleSystemConfig& config);
        ~ParticleSystem();

        void Initialize();
        void Update(float deltaTime);
        void Render(const glm::mat4& viewProjection, const glm::vec3& cameraPosition);

        // Control
        void Play();
        void Pause();
        void Stop();
        void Reset();

        // Emitter transform
        void SetPosition(const glm::vec3& position) { m_Position = position; }
        void SetRotation(const glm::quat& rotation) { m_Rotation = rotation; }
        
        glm::vec3 GetPosition() const { return m_Position; }

        // Settings
        void SetEmissionRate(float rate) { m_Config.EmissionRate = rate; }
        void SetStartColor(const glm::vec4& color) { m_Config.StartColor = color; }
        void SetEndColor(const glm::vec4& color) { m_Config.EndColor = color; }

        // Status
        bool IsPlaying() const { return m_IsPlaying; }
        uint32_t GetActiveParticleCount() const { return m_ActiveCount; }

    private:
        void Emit(uint32_t count);
        void SpawnParticle(Particle& particle);
        glm::vec3 GetRandomPositionInShape();
        glm::vec3 GetRandomDirection();

    private:
        ParticleSystemConfig m_Config;
        
        std::vector<Particle> m_Particles;
        uint32_t m_ActiveCount = 0;
        
        glm::vec3 m_Position = glm::vec3(0.0f);
        glm::quat m_Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        
        float m_EmissionAccumulator = 0.0f;
        bool m_IsPlaying = true;
        
        // GPU resources
        PT_Ref<Shader> m_ParticleShader;
        PT_Ref<VertexArray> m_VAO;
        PT_Ref<VertexBuffer> m_VBO;
        std::vector<float> m_InstanceData;
    };

    // Particle system collection
    class ParticleSystemCollection {
    public:
        void Add(PT_Ref<ParticleSystem> system) { m_Systems.push_back(system); }
        void Remove(PT_Ref<ParticleSystem> system);
        
        void Update(float deltaTime);
        void Render(const glm::mat4& viewProjection, const glm::vec3& cameraPosition);
        
    private:
        std::vector<PT_Ref<ParticleSystem>> m_Systems;
    };

}
