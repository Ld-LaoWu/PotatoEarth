#pragma once

#include "EarthCore/Core/PotatoEarthBase.h"
#include "EarthCore/Render/VertexArray.h"
#include "EarthCore/Render/Material.h"
#include "EarthCore/Core/Geospatial/Cartograph.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <string>

namespace PTEarth {

    // Entity types
    enum class EntityType {
        Point,
        Polyline,
        Polygon,
        Billboard,
        Label,
        Model,
        Box,
        Ellipse,
        Corridor
    };

    // Position property - can be static or time-dynamic
    class PositionProperty {
    public:
        virtual ~PositionProperty() = default;
        virtual glm::dvec3 GetValue(double time = 0.0) const = 0;
        virtual bool IsConstant() const { return true; }
    };

    // Constant position
    class ConstantPosition : public PositionProperty {
    public:
        ConstantPosition(const Cartographic& position);
        ConstantPosition(const glm::dvec3& cartesian);
        
        glm::dvec3 GetValue(double time = 0.0) const override { return m_Position; }
        
    private:
        glm::dvec3 m_Position;
    };

    // Sampled position (time-dynamic)
    struct PositionSample {
        double Time;
        glm::dvec3 Position;
    };

    class SampledPosition : public PositionProperty {
    public:
        void AddSample(double time, const glm::dvec3& position);
        glm::dvec3 GetValue(double time) const override;
        bool IsConstant() const override { return false; }
        
    private:
        std::vector<PositionSample> m_Samples;
    };

    // Graphics components
    struct PointGraphics {
        float PixelSize = 10.0f;
        glm::vec4 Color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
        bool Outline = true;
        glm::vec4 OutlineColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        float OutlineWidth = 1.0f;
        float ScaleByDistance = 1.0f;
    };

    struct PolylineGraphics {
        std::vector<glm::dvec3> Positions;
        glm::vec4 Color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        float Width = 2.0f;
        bool ClampToGround = false;
    };

    struct PolygonGraphics {
        std::vector<glm::dvec3> Positions;
        glm::vec4 Material = glm::vec4(0.0f, 0.5f, 1.0f, 0.5f);
        glm::vec4 OutlineColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
        float OutlineWidth = 1.0f;
        bool Fill = true;
        bool Outline = true;
    };

    struct BillboardGraphics {
        PT_Ref<Texture> Image;
        glm::vec2 Scale = glm::vec2(1.0f, 1.0f);
        glm::vec4 Color = glm::vec4(1.0f);
        glm::vec2 PixelOffset = glm::vec2(0.0f, 0.0f);
    };

    struct LabelGraphics {
        std::string Text;
        std::string Font = "16px sans-serif";
        glm::vec4 FillColor = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
        glm::vec4 OutlineColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        float OutlineWidth = 1.0f;
        glm::vec2 PixelOffset = glm::vec2(0.0f, -20.0f);
    };

    struct ModelGraphics {
        std::string URI;  // Path to glTF/glb file
        glm::vec3 Scale = glm::vec3(1.0f);
        float MinimumPixelSize = 0.0f;
        float MaximumScale = 20000.0f;
        bool RunAnimations = true;
    };

    // Entity class - similar to Cesium Entity
    class PTEARTH_API Entity {
    public:
        Entity(const std::string& id);
        ~Entity() = default;
        
        const std::string& GetID() const { return m_ID; }
        const std::string& GetName() const { return m_Name; }
        void SetName(const std::string& name) { m_Name = name; }
        
        // Position
        void SetPosition(PT_Ref<PositionProperty> position) { m_Position = position; }
        glm::dvec3 GetPosition(double time = 0.0) const {
            return m_Position ? m_Position->GetValue(time) : glm::dvec3(0.0);
        }
        
        // Orientation
        void SetOrientation(const glm::dquat& orientation) { m_Orientation = orientation; }
        glm::dquat GetOrientation() const { return m_Orientation; }
        
        // Graphics
        bool HasPoint() const { return m_ShowPoint; }
        bool HasPolyline() const { return m_ShowPolyline; }
        bool HasPolygon() const { return m_ShowPolygon; }
        bool HasBillboard() const { return m_ShowBillboard; }
        bool HasLabel() const { return m_ShowLabel; }
        bool HasModel() const { return m_ShowModel; }
        
        PointGraphics& GetPoint() { return m_Point; }
        PolylineGraphics& GetPolyline() { return m_Polyline; }
        PolygonGraphics& GetPolygon() { return m_Polygon; }
        BillboardGraphics& GetBillboard() { return m_Billboard; }
        LabelGraphics& GetLabel() { return m_Label; }
        ModelGraphics& GetModel() { return m_Model; }
        
        void SetPoint(const PointGraphics& point) { m_Point = point; m_ShowPoint = true; }
        void SetPolyline(const PolylineGraphics& polyline) { m_Polyline = polyline; m_ShowPolyline = true; }
        void SetPolygon(const PolygonGraphics& polygon) { m_Polygon = polygon; m_ShowPolygon = true; }
        void SetBillboard(const BillboardGraphics& billboard) { m_Billboard = billboard; m_ShowBillboard = true; }
        void SetLabel(const LabelGraphics& label) { m_Label = label; m_ShowLabel = true; }
        void SetModel(const ModelGraphics& model) { m_Model = model; m_ShowModel = true; }
        
        // Visibility
        bool IsShowing() const { return m_Show; }
        void Show(bool show) { m_Show = show; }
        
        // Selection
        bool IsSelected() const { return m_Selected; }
        void SetSelected(bool selected) { m_Selected = selected; }
        
    private:
        std::string m_ID;
        std::string m_Name;
        
        PT_Ref<PositionProperty> m_Position;
        glm::dquat m_Orientation = glm::dquat(1.0, 0.0, 0.0, 0.0);
        
        // Graphics
        PointGraphics m_Point;
        PolylineGraphics m_Polyline;
        PolygonGraphics m_Polygon;
        BillboardGraphics m_Billboard;
        LabelGraphics m_Label;
        ModelGraphics m_Model;
        
        bool m_ShowPoint = false;
        bool m_ShowPolyline = false;
        bool m_ShowPolygon = false;
        bool m_ShowBillboard = false;
        bool m_ShowLabel = false;
        bool m_ShowModel = false;
        
        bool m_Show = true;
        bool m_Selected = false;
    };

    // Entity collection
    class EntityCollection {
    public:
        Entity* Add(const std::string& id);
        Entity* GetById(const std::string& id);
        void Remove(const std::string& id);
        void RemoveAll();
        
        std::vector<Entity*> GetAll() const;
        std::vector<Entity*> GetVisible() const;
        
    private:
        std::unordered_map<std::string, PT_Ref<Entity>> m_Entities;
    };

}
