#pragma once
#include "EarthCore/Core/PotatoEarthBase.h"
#include <glm/vec3.hpp>

namespace PTEarth {
	/**
	* @brief ����
	*/
	class PTEARTH_API Ray final
	{
	public:
		Ray() = default;
		/**
		* @brief ����һ������
		*/
		Ray(const glm::dvec3& origin, const glm::dvec3& direction);
		const glm::dvec3& getOrigin() const noexcept { return  m_Origin; }
		const glm::dvec3& getDirection() const noexcept { return  m_Direction; }
		Ray operator-()const noexcept;
	private:
		glm::dvec3 m_Origin;
		glm::dvec3 m_Direction;
	};
}