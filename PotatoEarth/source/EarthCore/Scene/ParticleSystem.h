#pragma once

#include "EarthCore/Core/PotatoEarthBase.h"
#include "EarthCore/Render/Texture.h"
#include <glm/glm.hpp>
#include <vector>
#include <random>

namespace PTEarth {

    // Particle data
    struct Particle {
        glm::vec3 Position;
        glm::vec3 Velocity;
        glm::vec4 Color;
        float Size;
        float Rotation;
        float Life;
        float MaxLife;
        float NormalizedLife;  // 0.0 to 1.0
    };

    // Emitter shape
    enum class EmitterShape {
        Point,
        Sphere,
        Box,
        Cone,
        Circle
    };

    // Particle emitter configuration
    struct EmitterConfig {
        // Emission
        float EmissionRate = 100.0f;  // particles per second
        int BurstCount = 0;           // particles at start
        bool Looping = true;
        float Duration = 5.0f;        // emitter lifetime
        
        // Shape
        EmitterShape Shape = EmitterShape::Sphere;
        glm::vec3 ShapeExtents = glm::vec3(1.0f);
        float ShapeRadius = 1.0f;
        float ShapeAngle = 45.0f;     // for cone
        
        // Initial values
        glm::vec3 StartVelocity = glm::vec3(0.0f, 1.0f, 0.0f);
        float StartVelocityVariation = 0.5f;
        glm::vec4 StartColor = glm::vec4(1.0f);
        float StartColorVariation = 0.0f;
        float StartSize = 1.0f;
        float StartSizeVariation = 0.5f;
        float StartRotation = 0.0f;
        float StartRotationVariation = 180.0f;
        float StartLife = 5.0f;
        float StartLifeVariation = 1.0f;
        
        // Over lifetime
        glm::vec4 EndColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
        float EndSize = 0.0f;
        float EndRotation = 0.0f;
        
        // Physics
        glm::vec3 Gravity = glm::vec3(0.0f, -9.8f, 0.0f);
        float Drag = 0.1f;
        
        // Texture
        PT_Ref<Texture> ParticleTexture;
    };

    // Particle system
    class ParticleSystem {
    public:
        ParticleSystem();
        ~ParticleSystem();
        
        void Initialize(const EmitterConfig& config);
        void Shutdown();
        
        void Update(float deltaTime);
        void Render(const glm::mat4& viewProjection);
        
        // Control
        void Play();
        void Pause();
        void Stop();
        void Reset();
        
        // Burst emission
        void EmitBurst(int count);
        
        // Settings
        void SetEmitterPosition(const glm::vec3& position) { m_EmitterPosition = position; }
        void SetEmitterRotation(const glm::quat& rotation) { m_EmitterRotation = rotation; }
        
        bool IsPlaying() const { return m_Playing; }
        int GetActiveParticleCount() const { return m_ActiveParticles; }
        
    private:
        void EmitParticle();
        void UpdateParticle(Particle& particle, float deltaTime);
        glm::vec3 GenerateRandomPosition();
        glm::vec3 GenerateRandomVelocity();
        
    private:
        EmitterConfig m_Config;
        std::vector<Particle> m_Particles;
        
        glm::vec3 m_EmitterPosition = glm::vec3(0.0f);
        glm::quat m_EmitterRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        
        bool m_Playing = false;
        float m_EmitTimer = 0.0f;
        float m_EmitterTimer = 0.0f;
        int m_ActiveParticles = 0;
        
        std::mt19937 m_Random;
    };

    // Particle system collection
    class ParticleSystemCollection {
    public:
        void AddSystem(const std::string& name, PT_Ref<ParticleSystem> system);
        void RemoveSystem(const std::string& name);
        PT_Ref<ParticleSystem> GetSystem(const std::string& name);
        
        void Update(float deltaTime);
        void Render(const glm::mat4& viewProjection);
        
    private:
        std::unordered_map<std::string, PT_Ref<ParticleSystem>> m_Systems;
    };

    // Preset particle effects
    namespace ParticlePresets {
        EmitterConfig Fire();
        EmitterConfig Smoke();
        EmitterConfig Explosion();
        EmitterConfig Sparkles();
        EmitterConfig Rain();
        EmitterConfig Snow();
        EmitterConfig Dust();
        EmitterConfig Magic();
    }

}
