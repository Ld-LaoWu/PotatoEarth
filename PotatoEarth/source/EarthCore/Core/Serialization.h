#pragma once

#include "EarthCore/Core/PotatoEarthBase.h"
#include "EarthCore/Scene/Entity.h"
#include <json/json.h>
#include <string>

namespace PTEarth {

    // Scene serialization
    class SceneSerializer {
    public:
        // Serialize entire scene to JSON
        static std::string SerializeScene(class Scene* scene);
        static bool DeserializeScene(class Scene* scene, const std::string& jsonString);
        
        // Serialize individual entity
        static Json::Value SerializeEntity(Entity* entity);
        static PT_Ref<Entity> DeserializeEntity(const Json::Value& json);
        
        // Serialize graphics components
        static Json::Value SerializePointGraphics(const PointGraphics& point);
        static Json::Value SerializePolylineGraphics(const PolylineGraphics& polyline);
        static Json::Value SerializePolygonGraphics(const PolygonGraphics& polygon);
        static Json::Value SerializeBillboardGraphics(const BillboardGraphics& billboard);
        static Json::Value SerializeLabelGraphics(const LabelGraphics& label);
        static Json::Value SerializeModelGraphics(const ModelGraphics& model);
        
        // Deserialize graphics components
        static void DeserializePointGraphics(PointGraphics& point, const Json::Value& json);
        static void DeserializePolylineGraphics(PolylineGraphics& polyline, const Json::Value& json);
        static void DeserializePolygonGraphics(PolygonGraphics& polygon, const Json::Value& json);
        static void DeserializeBillboardGraphics(BillboardGraphics& billboard, const Json::Value& json);
        static void DeserializeLabelGraphics(LabelGraphics& label, const Json::Value& json);
        static void DeserializeModelGraphics(ModelGraphics& model, const Json::Value& json);
        
        // Save/Load to file
        static bool SaveToFile(class Scene* scene, const std::string& filepath);
        static bool LoadFromFile(class Scene* scene, const std::string& filepath);
    };

    // CZML serializer (Cesium language)
    class CZMLSerializer {
    public:
        // Serialize to CZML format
        static std::string SerializeToCZML(const EntityCollection& entities);
        static bool DeserializeFromCZML(EntityCollection& entities, const std::string& czmlString);
        
        // Entity to CZML packet
        static Json::Value EntityToPacket(Entity* entity);
        static PT_Ref<Entity> PacketToEntity(const Json::Value& packet);
        
        // Save/Load
        static bool SaveToCZMLFile(const EntityCollection& entities, const std::string& filepath);
        static bool LoadFromCZMLFile(EntityCollection& entities, const std::string& filepath);
    };

    // GeoJSON support
    class GeoJSONSerializer {
    public:
        // Parse GeoJSON
        static bool ParseGeoJSON(EntityCollection& entities, const std::string& jsonString);
        static bool ParseGeoJSONFile(EntityCollection& entities, const std::string& filepath);
        
        // Convert to GeoJSON
        static std::string ToGeoJSON(const EntityCollection& entities);
        static bool SaveToGeoJSONFile(const EntityCollection& entities, const std::string& filepath);
        
        // Feature types
        static PT_Ref<Entity> ParsePoint(const Json::Value& geometry, const Json::Value& properties);
        static PT_Ref<Entity> ParseLineString(const Json::Value& geometry, const Json::Value& properties);
        static PT_Ref<Entity> ParsePolygon(const Json::Value& geometry, const Json::Value& properties);
    };

    // KML support
    class KMLSerializer {
    public:
        // Parse KML
        static bool ParseKML(EntityCollection& entities, const std::string& kmlString);
        static bool ParseKMLFile(EntityCollection& entities, const std::string& filepath);
        
        // Convert to KML
        static std::string ToKML(const EntityCollection& entities);
        static bool SaveToKMLFile(const EntityCollection& entities, const std::string& filepath);
    };

    // Project file format
    struct ProjectSettings {
        std::string Name = "Untitled Project";
        std::string Description = "";
        std::string Author = "";
        
        // Camera settings
        double CameraLongitude = 0.0;
        double CameraLatitude = 0.0;
        double CameraHeight = 20000000.0;
        
        // Time settings
        double StartTime = 0.0;
        double CurrentTime = 0.0;
        double Multiplier = 1.0;
        
        // Display settings
        bool ShowGlobe = true;
        bool ShowAtmosphere = true;
        bool ShowSky = true;
        
        // Layers
        std::vector<std::string> ImageryLayers;
        std::string TerrainProvider;
    };

    class ProjectManager {
    public:
        // Current project
        void NewProject(const std::string& name);
        void OpenProject(const std::string& filepath);
        void SaveProject();
        void SaveProjectAs(const std::string& filepath);
        
        // Settings
        ProjectSettings& GetSettings() { return m_Settings; }
        const std::string& GetProjectPath() const { return m_ProjectPath; }
        bool HasUnsavedChanges() const { return m_UnsavedChanges; }
        
        // Auto-save
        void EnableAutoSave(bool enable) { m_AutoSave = enable; }
        void SetAutoSaveInterval(float minutes) { m_AutoSaveInterval = minutes; }
        
    private:
        ProjectSettings m_Settings;
        std::string m_ProjectPath;
        bool m_UnsavedChanges = false;
        
        bool m_AutoSave = false;
        float m_AutoSaveInterval = 5.0f;  // minutes
    };

}
