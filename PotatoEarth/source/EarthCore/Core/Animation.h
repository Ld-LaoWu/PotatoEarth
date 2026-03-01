#pragma once

#include "EarthCore/Core/PotatoEarthBase.h"
#include <glm/glm.hpp>
#include <vector>
#include <math.h>

namespace PTEarth {

    // Bezier curve interpolation
    class BezierCurve {
    public:
        BezierCurve(const std::vector<glm::dvec3>& controlPoints);
        
        glm::dvec3 Evaluate(double t) const;
        glm::dvec3 EvaluateDerivative(double t) const;
        
        // Sample points along curve
        std::vector<glm::dvec3> Sample(int segments) const;
        
        // Arc length
        double GetLength() const;
        double GetLength(double tStart, double tEnd) const;
        
        // Get t for given arc length
        double GetTAtLength(double length) const;
        
    private:
        std::vector<glm::dvec3> m_ControlPoints;
        int m_Degree;
    };

    // Catmull-Rom spline (smooth interpolation through points)
    class CatmullRomSpline {
    public:
        CatmullRomSpline(const std::vector<glm::dvec3>& points, float alpha = 0.5f);
        
        glm::dvec3 Evaluate(double t) const;
        glm::dvec3 EvaluateDerivative(double t) const;
        
        // Add control point
        void AddPoint(const glm::dvec3& point);
        
        // Sample points
        std::vector<glm::dvec3> Sample(int segments) const;
        
    private:
        std::vector<glm::dvec3> m_Points;
        float m_Alpha;
    };

    // Hermite spline (with tangents)
    class HermiteSpline {
    public:
        struct ControlPoint {
            glm::dvec3 Position;
            glm::dvec3 Tangent;
        };
        
        HermiteSpline(const std::vector<ControlPoint>& points);
        
        glm::dvec3 Evaluate(double t) const;
        glm::dvec3 EvaluateDerivative(double t) const;
        
        // Sample points
        std::vector<glm::dvec3> Sample(int segments) const;
        
    private:
        std::vector<ControlPoint> m_Points;
    };

    // Path for camera or entity movement
    class Path {
    public:
        enum class InterpolationType {
            Linear,
            Bezier,
            CatmullRom,
            Hermite
        };
        
        Path(InterpolationType type = InterpolationType::CatmullRom);
        
        void AddPoint(const glm::dvec3& point);
        void AddPoint(const glm::dvec3& point, const glm::dvec3& tangent);
        void Clear();
        
        glm::dvec3 GetPosition(double t) const;
        glm::dvec3 GetTangent(double t) const;
        glm::dquat GetOrientation(double t) const;
        
        // Arc length parameterization
        glm::dvec3 GetPositionAtLength(double length) const;
        double GetTotalLength() const;
        
        // Sample points along path
        std::vector<glm::dvec3> Sample(int segments) const;
        
    private:
        InterpolationType m_Type;
        std::vector<glm::dvec3> m_Points;
        std::vector<glm::dvec3> m_Tangents;
        
        std::unique_ptr<CatmullRomSpline> m_Spline;
        double m_TotalLength = 0.0;
    };

    // Camera path animation
    class CameraPathAnimator {
    public:
        struct Keyframe {
            double Time;  // 0.0 to 1.0
            glm::dvec3 Position;
            glm::dvec3 LookAt;
            double FOV;
        };
        
        void AddKeyframe(const Keyframe& keyframe);
        void Clear();
        
        // Evaluate camera at time t (0.0 to 1.0)
        void Evaluate(double t, glm::dvec3& position, glm::dvec3& lookAt, double& fov) const;
        
        // Play animation
        void Play(float duration);
        void Pause();
        void Stop();
        void Update(float deltaTime);
        
        bool IsPlaying() const { return m_Playing; }
        double GetCurrentTime() const { return m_CurrentTime; }
        
    private:
        std::vector<Keyframe> m_Keyframes;
        bool m_Playing = false;
        double m_CurrentTime = 0.0;
        double m_Duration = 1.0;
    };

    // Flight path generator
    class FlightPathGenerator {
    public:
        // Generate smooth flight path between two positions
        static Path GenerateFlightPath(
            const Cartographic& start,
            const Cartographic& end,
            double altitude = -1.0,  // -1 = auto
            double peakHeight = -1.0  // -1 = auto
        );
        
        // Generate orbit path around point
        static Path GenerateOrbitPath(
            const Cartographic& center,
            double radius,
            int segments = 64
        );
        
        // Generate spiral path
        static Path GenerateSpiralPath(
            const Cartographic& center,
            double startRadius,
            double endRadius,
            double height,
            int turns = 3,
            int segmentsPerTurn = 32
        );
    };

    // Easing functions for animations
    namespace Easing {
        double Linear(double t);
        double EaseInQuad(double t);
        double EaseOutQuad(double t);
        double EaseInOutQuad(double t);
        double EaseInCubic(double t);
        double EaseOutCubic(double t);
        double EaseInOutCubic(double t);
        double EaseInSine(double t);
        double EaseOutSine(double t);
        double EaseInOutSine(double t);
        double EaseInExpo(double t);
        double EaseOutExpo(double t);
        double EaseInOutExpo(double t);
    }

}
