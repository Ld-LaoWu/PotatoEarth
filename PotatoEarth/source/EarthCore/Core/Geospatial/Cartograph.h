#pragma once
#include "EarthCore/Core/Utility/Math/PTMath.h"
#include "EarthCore/Core/PotatoEarthBase.h"
namespace PTEarth{
	class PTEARTH_API Cartographic final 
	{
	public:
		// ����ʵ��
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

		// ����  ����
		double longitude;
		// γ��  ����
		double latitude;
		// �߶�  ��
		double height;
	};
}