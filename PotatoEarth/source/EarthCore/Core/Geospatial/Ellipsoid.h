#pragma once
#include "EarthCore/Core/PotatoEarthBase.h"

#include <glm/vec3.hpp>
#include <optional>

#include "EarthCore/Core/Utility/Math/PTMath.h"
#include "Cartograph.h"


namespace PTEarth {
	/*
	* ������ �ѿ�������ϵ���涨��
	* `(x / a)^2 + (y / b)^2 + (z / c)^2 = 1` ������ı�﷽��
	*/
	class PTEARTH_API Ellipsoid
	{
	public:
		static const Ellipsoid WGS84;   // Ĭ��ʹ�õ�������

		// ʵ������

		constexpr Ellipsoid(const glm::dvec3& radii ) noexcept
			: m_Radii(radii),
			  m_RadiiSquared(radii.x * radii.x, radii.y * radii.y, radii.z * radii.z),
			  m_OneOverRadii(1.0 / radii.x, 1.0/ radii.y, 1.0/ radii.z),
			  m_OneOverRadiiSquared(
				  1.0 / (radii.x * radii.x),
				  1.0 / (radii.y * radii.y),
				  1.0 / (radii.z * radii.z)),
			  m_CenterToleranceSquared( PTMath::Epsilon1 ){}

		constexpr Ellipsoid(double x, double y, double z) noexcept
			: Ellipsoid(glm::dvec3(x, y, z)) {}

		constexpr const glm::dvec3& getRadii() const noexcept { return this->m_Radii; }

		/*
		* @brief ����������������е�ƽ��ķ���
		* @param position �ѿ�������ϵ
		*/
		glm::dvec3 geodeticSurfaceNormal(const glm::dvec3& position) const noexcept;
		glm::dvec3 geodeticSurfaceNormal(const Cartographic& cartographic) const noexcept;

		/*
		* @brief �ѿ���ת��γ��
		* @param cartesian �ѿ�������
		*/
		std::optional<Cartographic> cartesianToCartographic(const glm::dvec3& cartesian) const noexcept;
		/*
		* @brief ��γ��ת�ѿ���
		* @param cartesian ��γ������
		*/
		glm::dvec3 cartographicToCartesian(const Cartographic& cartographic) const noexcept;
		/*
		* @brief�ӿռ䵽�������������
		* @param cartesian �ѿ�������
		*/
		std::optional<glm::dvec3> scaleToGeodeticSurface(const glm::dvec3& cartesian) const noexcept;
		/*
		* @brief ��ȡ�������İ뾶
		*/
		constexpr double getMaximumRadius() const noexcept 
		{
			return glm::max(this->m_Radii.x, glm::max(this->m_Radii.y, this->m_Radii.z));
		}

		/*
		* @brief ��ȡ������С�İ뾶
		*/
		constexpr double getMinimumRadius() const noexcept
		{
			return glm::min(this->m_Radii.x, glm::min(this->m_Radii.y, this->m_Radii.z));
		}

		constexpr bool operator==(const Ellipsoid& rhs) const noexcept {
			return this->m_Radii == rhs.m_Radii;
		}

		constexpr bool operator!=(const Ellipsoid& rhs) const noexcept {
			return !(*this == rhs);
		}
	private:
		glm::dvec3 m_Radii;
		glm::dvec3 m_RadiiSquared;
		glm::dvec3 m_OneOverRadii;
		glm::dvec3 m_OneOverRadiiSquared;
		
		double m_CenterToleranceSquared;
	};
}