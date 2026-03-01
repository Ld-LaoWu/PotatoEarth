#include "potatopch.h"
#include "Entity.h"

#include "EarthCore/Core/Geospatial/Ellipsoid.h"

namespace PTEarth {

    // ConstantPosition
    ConstantPosition::ConstantPosition(const Cartographic& position) {
        m_Position = Ellipsoid::WGS84.cartographicToCartesian(position);
    }

    ConstantPosition::ConstantPosition(const glm::dvec3& cartesian) : m_Position(cartesian) {
    }

    // SampledPosition
    void SampledPosition::AddSample(double time, const glm::dvec3& position) {
        m_Samples.push_back({time, position});
        // Keep sorted by time
        std::sort(m_Samples.begin(), m_Samples.end(), 
            [](const PositionSample& a, const PositionSample& b) {
                return a.Time < b.Time;
            });
    }

    glm::dvec3 SampledPosition::GetValue(double time) const {
        if (m_Samples.empty()) return glm::dvec3(0.0);
        if (m_Samples.size() == 1) return m_Samples[0].Position;
        
        // Find surrounding samples
        for (size_t i = 0; i < m_Samples.size() - 1; ++i) {
            if (time >= m_Samples[i].Time && time <= m_Samples[i + 1].Time) {
                // Linear interpolation
                double t = (time - m_Samples[i].Time) / (m_Samples[i + 1].Time - m_Samples[i].Time);
                return glm::mix(m_Samples[i].Position, m_Samples[i + 1].Position, t);
            }
        }
        
        // Return last sample if time is beyond range
        return m_Samples.back().Position;
    }

    // Entity
    Entity::Entity(const std::string& id) : m_ID(id), m_Name(id) {
    }

    // EntityCollection
    Entity* EntityCollection::Add(const std::string& id) {
        auto entity = CreateRef<Entity>(id);
        m_Entities[id] = entity;
        return entity.get();
    }

    Entity* EntityCollection::GetById(const std::string& id) {
        auto it = m_Entities.find(id);
        if (it != m_Entities.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    void EntityCollection::Remove(const std::string& id) {
        m_Entities.erase(id);
    }

    void EntityCollection::RemoveAll() {
        m_Entities.clear();
    }

    std::vector<Entity*> EntityCollection::GetAll() const {
        std::vector<Entity*> result;
        for (auto& pair : m_Entities) {
            result.push_back(pair.second.get());
        }
        return result;
    }

    std::vector<Entity*> EntityCollection::GetVisible() const {
        std::vector<Entity*> result;
        for (auto& pair : m_Entities) {
            if (pair.second->IsShowing()) {
                result.push_back(pair.second.get());
            }
        }
        return result;
    }

}
