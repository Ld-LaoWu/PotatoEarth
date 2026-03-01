#pragma once

#include "EarthCore/Core/PotatoEarthBase.h"
#include <string>
#include <vector>

namespace PTEarth {

    // Julian Date - astronomical time representation
    class JulianDate {
    public:
        JulianDate() : m_DayNumber(0.0) {}
        JulianDate(double dayNumber) : m_DayNumber(dayNumber) {}
        
        double GetDayNumber() const { return m_DayNumber; }
        
        // From Gregorian calendar
        static JulianDate FromDate(int year, int month, int day, int hour = 0, int minute = 0, double second = 0.0);
        
        // To seconds
        double ToSeconds() const { return m_DayNumber * 86400.0; }
        static JulianDate FromSeconds(double seconds) { return JulianDate(seconds / 86400.0); }
        
        // Operators
        JulianDate operator+(double days) const { return JulianDate(m_DayNumber + days); }
        JulianDate operator-(double days) const { return JulianDate(m_DayNumber - days); }
        double operator-(const JulianDate& other) const { return m_DayNumber - other.m_DayNumber; }
        bool operator<(const JulianDate& other) const { return m_DayNumber < other.m_DayNumber; }
        bool operator>(const JulianDate& other) const { return m_DayNumber > other.m_DayNumber; }
        
        std::string ToString() const;
        
    private:
        double m_DayNumber;
    };

    // Clock - controls simulation time
    class Clock {
    public:
        enum class ClockStep {
            TICK_DEPENDENT,    // Advance by tick delta * multiplier
            SYSTEM_CLOCK       // Advance to system time
        };
        
        enum class ClockRange {
            UNBOUNDED,         // No limit
            CLAMPED,           // Clamp to start/stop
            LOOP               // Loop around
        };
        
        Clock();
        
        // Initialize
        void Initialize();
        
        // Update clock
        void Tick(float deltaTime);
        
        // Get current time
        JulianDate GetCurrentTime() const { return m_CurrentTime; }
        void SetCurrentTime(const JulianDate& time) { m_CurrentTime = time; }
        
        // Time control
        void Play() { m_ShouldAnimate = true; }
        void Pause() { m_ShouldAnimate = false; }
        void Stop();
        bool IsPlaying() const { return m_ShouldAnimate; }
        
        // Time multiplier (1.0 = real time, >1 = faster, <0 = reverse)
        float GetMultiplier() const { return m_Multiplier; }
        void SetMultiplier(float multiplier) { m_Multiplier = multiplier; }
        
        // Time range
        void SetStartTime(const JulianDate& time) { m_StartTime = time; }
        void SetStopTime(const JulianDate& time) { m_StopTime = time; }
        JulianDate GetStartTime() const { return m_StartTime; }
        JulianDate GetStopTime() const { return m_StopTime; }
        
        // Clock step mode
        void SetClockStep(ClockStep step) { m_ClockStep = step; }
        ClockStep GetClockStep() const { return m_ClockStep; }
        
        // Clock range
        void SetClockRange(ClockRange range) { m_ClockRange = range; }
        ClockRange GetClockRange() const { return m_ClockRange; }
        
    private:
        JulianDate m_CurrentTime;
        JulianDate m_StartTime;
        JulianDate m_StopTime;
        
        float m_Multiplier = 1.0f;
        bool m_ShouldAnimate = true;
        
        ClockStep m_ClockStep = ClockStep::TICK_DEPENDENT;
        ClockRange m_ClockRange = ClockRange::UNBOUNDED;
    };

    // Time interval
    template<typename T>
    struct TimeInterval {
        JulianDate Start;
        JulianDate Stop;
        T Value;
    };

    // Sampled property for time-dynamic values
    template<typename T>
    class SampledProperty {
    public:
        void AddSample(const JulianDate& time, const T& value) {
            m_Samples.push_back({time, T()});
            m_Samples.back().Value = value;
            SortSamples();
        }
        
        T GetValue(const JulianDate& time) const {
            if (m_Samples.empty()) return T();
            if (m_Samples.size() == 1) return m_Samples[0].Value;
            
            // Find surrounding samples
            for (size_t i = 0; i < m_Samples.size() - 1; ++i) {
                if (time >= m_Samples[i].Start && time <= m_Samples[i + 1].Start) {
                    // Interpolate
                    double t = (time.GetDayNumber() - m_Samples[i].Start.GetDayNumber()) / 
                               (m_Samples[i + 1].Start.GetDayNumber() - m_Samples[i].Start.GetDayNumber());
                    return Interpolate(m_Samples[i].Value, m_Samples[i + 1].Value, t);
                }
            }
            
            return m_Samples.back().Value;
        }
        
        bool IsConstant() const { return m_Samples.size() <= 1; }
        
    private:
        void SortSamples() {
            std::sort(m_Samples.begin(), m_Samples.end(),
                [](const TimeInterval<T>& a, const TimeInterval<T>& b) {
                    return a.Start < b.Start;
                });
        }
        
        T Interpolate(const T& a, const T& b, double t) const {
            return a + (b - a) * t;
        }
        
        std::vector<TimeInterval<T>> m_Samples;
    };

    // Specialized interpolation for quaternions
    template<>
    inline glm::quat SampledProperty<glm::quat>::Interpolate(const glm::quat& a, const glm::quat& b, double t) const {
        return glm::slerp(a, b, (float)t);
    }

}
