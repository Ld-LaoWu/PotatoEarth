#pragma once

#include "EarthCore/Core/PotatoEarthBase.h"
#include "Entity.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <map>
#include <string>

namespace PTEarth {

    // Animation interpolation types
    enum class AnimationInterpolation {
        Linear,
        Smooth,
        Accelerate,
        Decelerate,
        Bounce,
        Step
    };

    // Keyframe
    template<typename T>
    struct Keyframe {
        double Time;  // Seconds from animation start
        T Value;
        AnimationInterpolation Interpolation = AnimationInterpolation::Linear;
    };

    // Animation curve
    template<typename T>
    class AnimationCurve {
    public:
        void AddKeyframe(const Keyframe<T>& keyframe);
        T Evaluate(double time) const;
        
        double GetDuration() const {
            if (m_Keyframes.empty()) return 0.0;
            return m_Keyframes.back().Time;
        }
        
        bool IsEmpty() const { return m_Keyframes.empty(); }

    private:
        T Interpolate(const T& a, const T& b, double t, AnimationInterpolation type) const;
        
    private:
        std::vector<Keyframe<T>> m_Keyframes;
    };

    // Animation clip - collection of curves for an entity
    struct AnimationClip {
        std::string Name;
        double Duration = 0.0;
        bool Loop = true;
        
        // Position animation
        AnimationCurve<glm::dvec3> PositionCurve;
        
        // Rotation animation
        AnimationCurve<glm::dquat> RotationCurve;
        
        // Scale animation
        AnimationCurve<glm::vec3> ScaleCurve;
        
        // Visibility
        AnimationCurve<bool> VisibilityCurve;
    };

    // Animation player
    class AnimationPlayer {
    public:
        void Play(PT_Ref<AnimationClip> clip);
        void Pause();
        void Stop();
        void Resume();
        
        void Update(float deltaTime);
        void ApplyToEntity(Entity* entity);
        
        // Control
        void SetTime(double time) { m_CurrentTime = time; }
        void SetSpeed(float speed) { m_Speed = speed; }
        void SetLoop(bool loop) { if (m_CurrentClip) m_CurrentClip->Loop = loop; }
        
        double GetTime() const { return m_CurrentTime; }
        float GetSpeed() const { return m_Speed; }
        bool IsPlaying() const { return m_IsPlaying; }
        
    private:
        PT_Ref<AnimationClip> m_CurrentClip;
        double m_CurrentTime = 0.0;
        float m_Speed = 1.0f;
        bool m_IsPlaying = false;
    };

    // Animation controller - manages multiple animations
    class AnimationController {
    public:
        void AddClip(const std::string& name, PT_Ref<AnimationClip> clip);
        PT_Ref<AnimationClip> GetClip(const std::string& name);
        
        void Play(const std::string& name);
        void CrossFade(const std::string& name, float fadeTime);
        void Blend(const std::string& name1, const std::string& name2, float weight);
        
        void Update(float deltaTime);
        void ApplyToEntity(Entity* entity);
        
    private:
        std::map<std::string, PT_Ref<AnimationClip>> m_Clips;
        AnimationPlayer m_Player;
        
        // Cross-fading
        PT_Ref<AnimationClip> m_PreviousClip;
        float m_FadeTime = 0.0f;
        float m_FadeProgress = 0.0f;
        bool m_IsCrossFading = false;
    };

    // glTF animation loader
    class GLTFAnimationLoader {
    public:
        static std::vector<PT_Ref<AnimationClip>> LoadAnimations(const std::string& gltfPath);
    };

}
