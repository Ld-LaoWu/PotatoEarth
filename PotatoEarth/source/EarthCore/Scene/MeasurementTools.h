#pragma once

#include "EarthCore/Core/PotatoEarthBase.h"
#include "EarthCore/Core/Geospatial/Cartograph.h"
#include <glm/glm.hpp>
#include <vector>

namespace PTEarth {

    // Measurement types
    enum class MeasurementType {
        Distance,
        Area,
        Height,
        Angle
    };

    // Measurement result
    struct MeasurementResult {
        double Value;
        std::string Unit;
        bool IsValid;
    };

    // Measurement tool
    class MeasurementTool {
    public:
        virtual ~MeasurementTool() = default;
        virtual MeasurementType GetType() const = 0;
        virtual void AddPoint(const Cartographic& point) = 0;
        virtual void RemoveLastPoint() = 0;
        virtual void Clear() = 0;
        virtual MeasurementResult GetResult() const = 0;
        virtual std::vector<glm::dvec3> GetPoints() const = 0;
    };

    // Distance measurement
    class DistanceMeasurement : public MeasurementTool {
    public:
        MeasurementType GetType() const override { return MeasurementType::Distance; }
        
        void AddPoint(const Cartographic& point) override {
            m_Points.push_back(point);
        }
        
        void RemoveLastPoint() override {
            if (!m_Points.empty()) m_Points.pop_back();
        }
        
        void Clear() override {
            m_Points.clear();
        }
        
        MeasurementResult GetResult() const override {
            if (m_Points.size() < 2) return {0.0, "m", false};
            
            double totalDistance = 0.0;
            for (size_t i = 0; i < m_Points.size() - 1; ++i) {
                totalDistance += CalculateDistance(m_Points[i], m_Points[i + 1]);
            }
            
            // Format unit
            if (totalDistance > 1000.0) {
                return {totalDistance / 1000.0, "km", true};
            }
            return {totalDistance, "m", true};
        }
        
        std::vector<glm::dvec3> GetPoints() const override {
            std::vector<glm::dvec3> result;
            // Convert to Cartesian
            return result;
        }

    private:
        double CalculateDistance(const Cartographic& p1, const Cartographic& p2) const {
            // Haversine formula for great-circle distance
            double dlon = p2.longitude - p1.longitude;
            double dlat = p2.latitude - p1.latitude;
            double a = sin(dlat / 2) * sin(dlat / 2) +
                       cos(p1.latitude) * cos(p2.latitude) *
                       sin(dlon / 2) * sin(dlon / 2);
            double c = 2 * atan2(sqrt(a), sqrt(1 - a));
            return WGS84_A * c;
        }
        
    private:
        std::vector<Cartographic> m_Points;
    };

    // Area measurement
    class AreaMeasurement : public MeasurementTool {
    public:
        MeasurementType GetType() const override { return MeasurementType::Area; }
        
        void AddPoint(const Cartographic& point) override {
            m_Points.push_back(point);
        }
        
        void RemoveLastPoint() override {
            if (!m_Points.empty()) m_Points.pop_back();
        }
        
        void Clear() override {
            m_Points.clear();
        }
        
        MeasurementResult GetResult() const override {
            if (m_Points.size() < 3) return {0.0, "m", false};
            
            double area = CalculateArea();
            
            if (area > 1000000.0) {
                return {area / 1000000.0, "km", true};
            }
            return {area, "m", true};
        }
        
        std::vector<glm::dvec3> GetPoints() const override {
            return std::vector<glm::dvec3>();
        }

    private:
        double CalculateArea() const {
            // Spherical polygon area calculation
            double total = 0.0;
            size_t n = m_Points.size();
            
            for (size_t i = 0; i < n; ++i) {
                const Cartographic& p1 = m_Points[i];
                const Cartographic& p2 = m_Points[(i + 1) % n];
                
                total += (p2.longitude - p1.longitude) *
                         (2 + sin(p1.latitude) + sin(p2.latitude));
            }
            
            return abs(total * WGS84_A * WGS84_A / 2.0);
        }
        
    private:
        std::vector<Cartographic> m_Points;
    };

    // Measurement manager
    class MeasurementManager {
    public:
        void SetActiveTool(MeasurementType type);
        void AddPoint(const Cartographic& point);
        void RemoveLastPoint();
        void Clear();
        void Finish();
        
        MeasurementResult GetCurrentResult() const;
        bool IsActive() const { return m_ActiveTool != nullptr; }
        
    private:
        PT_Ref<MeasurementTool> m_ActiveTool;
        std::vector<PT_Ref<MeasurementTool>> m_CompletedMeasurements;
    };

}
