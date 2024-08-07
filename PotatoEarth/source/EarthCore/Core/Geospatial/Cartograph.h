#pragma once
#include "EarthCore/Core/Utility/Math/PTMath.h"
#include "EarthCore/Core/PotatoEarthBase.h"
namespace PTEarth{
	class PTEARTH_API Cartographic final 
	{
	public:
		// 经度实例
		constexpr Cartographic(
			double longitudeRadians,
			double latitudeRadians,
			double heightMeters = 0.0) noexcept
			:
			longitude(longitudeRadians),
			latitude(latitudeRadians),
			height(heightMeters)
		{}

		static constexpr Cartographic fromDegrees(
			double longitudeDegrees,
			double latitudeDegrees,
			double heightMeters = 0.0) noexcept
		{
			return Cartographic(
				PTMath::degreesToRadians(longitudeDegrees),
				PTMath::degreesToRadians(latitudeDegrees),
				heightMeters
			);
		}

		// 经度  弧度
		double longitude;
		// 纬度  弧度
		double latitude;
		// 高度  米
		double height;
	};
}