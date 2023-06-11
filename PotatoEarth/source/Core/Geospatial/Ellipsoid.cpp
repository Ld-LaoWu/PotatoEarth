#include "potatopch.h"
#include "Ellipsoid.h"

#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

namespace PTEarth {

	const Ellipsoid Ellipsoid::WGS84(6378137.0, 6378137.0, 6356752.3142451793);

	glm::dvec3 Ellipsoid::geodeticSurfaceNormal(const glm::dvec3& position) const noexcept
	{
		return glm::normalize(position * this->m_OneOverRadiiSquared);
	}
	glm::dvec3 Ellipsoid::geodeticSurfaceNormal(const Cartographic& cartographic) const noexcept
	{
		const double logitude = cartographic.longitude;
		const double latitude = cartographic.latitude;
		const double cosLatitude = glm::cos(latitude);

		return glm::normalize(glm::dvec3(
			cosLatitude * glm::cos(logitude),
			cosLatitude * glm::sin(logitude),
			glm::sin(latitude)
		));
	}
	std::optional<Cartographic> Ellipsoid::cartesianToCartographic(const glm::dvec3& cartesian) const noexcept
	{
		std::optional<glm::dvec3> p = this->scaleToGeodeticSurface(cartesian);
		if (!p) {
			return std::optional<Cartographic>();
		}

		const glm::dvec3 n = this->geodeticSurfaceNormal(p.value());
		const glm::dvec3 h = cartesian - p.value();

		const double longitude = glm::atan(n.y, n.x);
		const double latitude = glm::asin(n.z);
		const double height = PTMath::sign(glm::dot(h, cartesian) * glm::length(h));

		return Cartographic(longitude, latitude, height);

	}
	glm::dvec3 Ellipsoid::cartographicToCartesian(const Cartographic& cartographic) const noexcept
	{
		glm::dvec3 n = this->geodeticSurfaceNormal(cartographic);
		glm::dvec3 k = this->m_RadiiSquared * n;
		const double gamma = sqrt(glm::dot(n, k));

		k /= gamma;
		n *= cartographic.height;

		return k + n;
	}

	// 牛顿拉普拉斯迭代方程
	std::optional<glm::dvec3> Ellipsoid::scaleToGeodeticSurface(const glm::dvec3& cartesian) const noexcept
	{
		const double positionX = cartesian.x;
		const double positionY = cartesian.y;
		const double positionZ = cartesian.z;

		const double oneOverRadiiX = this->m_OneOverRadii.x;
		const double oneOverRadiiY = this->m_OneOverRadii.y;
		const double oneOverRadiiZ = this->m_OneOverRadii.z;

		const double  x2 = positionX * positionX * oneOverRadiiX * oneOverRadiiX;
		const double  y2 = positionY * positionY * oneOverRadiiY * oneOverRadiiY;
		const double  z2 = positionZ * positionZ * oneOverRadiiZ * oneOverRadiiZ;

		const double squaredNorm = x2 + y2 + z2;
		const double ratio = sqrt(1.0 / squaredNorm);

		glm::dvec3 intersection = cartesian * ratio;
		if (squaredNorm < this->m_CenterToleranceSquared)
		{
			return std::isfinite(ratio) ? std::optional<glm::dvec3>() : intersection;
		}

		const double oneOverRadiiSquaredX = this->m_OneOverRadiiSquared.x;
		const double oneOverRadiiSquaredY = this->m_OneOverRadiiSquared.y;
		const double oneOverRadiiSquaredZ = this->m_OneOverRadiiSquared.z;

		const glm::dvec3 gradient(
			intersection.x * oneOverRadiiSquaredX * 2.0,
			intersection.y * oneOverRadiiSquaredY * 2.0,
			intersection.z * oneOverRadiiSquaredZ * 2.0
		);

		double lambda = ((1.0 - ratio) * glm::length(cartesian)) / (0.5 * glm::length(gradient));
		double correction = 0.0;

		double func;
		double denominator;
		double xMultiplier;
		double yMultiplier;
		double zMultiplier;
		double xMultiplier2;
		double yMultiplier2;
		double zMultiplier2;
		double xMultiplier3;
		double yMultiplier3;
		double zMultiplier3;

		do {
			lambda -= correction;

			xMultiplier = 1.0 / (1.0 + lambda * oneOverRadiiSquaredX);
			yMultiplier = 1.0 / (1.0 + lambda * oneOverRadiiSquaredY);
			zMultiplier = 1.0 / (1.0 + lambda * oneOverRadiiSquaredZ);

			xMultiplier2 = xMultiplier * xMultiplier;
			yMultiplier2 = yMultiplier * yMultiplier;
			zMultiplier2 = zMultiplier * zMultiplier;

			xMultiplier3 = xMultiplier2 * xMultiplier;
			yMultiplier3 = yMultiplier2 * yMultiplier;
			zMultiplier3 = zMultiplier2 * zMultiplier;

			func = x2 * xMultiplier2 + y2 * yMultiplier2 + z2 * zMultiplier2 - 1.0;

			denominator = x2 * xMultiplier3 * oneOverRadiiSquaredX +
				y2 * yMultiplier3 * oneOverRadiiSquaredY +
				z2 * zMultiplier3 * oneOverRadiiSquaredZ;

			const double derivative = -2.0 * denominator;

			correction = func / derivative;
		} while (glm::abs(func) > PTMath::Epsilon12);

		return std::optional<glm::dvec3>();
	}
}