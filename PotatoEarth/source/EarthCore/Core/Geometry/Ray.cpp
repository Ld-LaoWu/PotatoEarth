#include "potatopch.h"
#include "Ray.h"
#include "EarthCore/Core/Utility/Math/PTMath.h"

namespace PTEarth {
	Ray::Ray(const glm::dvec3& origin, const glm::dvec3& direction)
		:m_Origin(origin), m_Direction(direction)
	{
		
	}
	Ray Ray::operator-() const noexcept
	{
		return Ray(this->m_Origin,-this->m_Direction);
	}
}