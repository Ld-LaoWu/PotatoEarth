#pragma once
#include "EarthCore/Core/PotatoEarthBase.h"
#include <glm/gtc/epsilon.hpp>

namespace PTEarth {
	class PTEARTH_API PTMath final 
	{
	public:
		// 0.1
		static constexpr double Epsilon1 = 1e-1;
		// 0.01
		static constexpr double Epsilon2 = 1e-2;
		// 0.001
		static constexpr double Epsilon3 = 1e-3;
		// 0.0001
		static constexpr double Epsilon4 = 1e-4;
		// 0.00001
		static constexpr double Epsilon5 = 1e-5;
		// 0.000001
		static constexpr double Epsilon6 = 1e-6;
		// 0.0000001
		static constexpr double Epsilon7 = 1e-7;
		// 0.00000001
		static constexpr double Epsilon8 = 1e-8;
		// 0.000000001
		static constexpr double Epsilon9 = 1e-9;
		// 0.0000000001
		static constexpr double Epsilon10 = 1e-10;
		// 0.00000000001
		static constexpr double Epsilon11 = 1e-11;
		// 0.000000000001
		static constexpr double Epsilon12 = 1e-12;
		// 0.0000000000001
		static constexpr double Epsilon13 = 1e-13;
		// 0.00000000000001
		static constexpr double Epsilon14 = 1e-14;
		// 0.000000000000001
		static constexpr double Epsilon15 = 1e-15;
		// 0.0000000000000001
		static constexpr double Epsilon16 = 1e-16;
		// 0.00000000000000001
		static constexpr double Epsilon17 = 1e-17;
		// 0.000000000000000001
		static constexpr double Epsilon18 = 1e-18;
		// 0.0000000000000000001
		static constexpr double Epsilon19 = 1e-19;
		// 0.00000000000000000001
		static constexpr double Epsilon20 = 1e-20;
		// 0.000000000000000000001 ��21λ
		static constexpr double Epsilon21 = 1e-21;

		// Բ����
		static constexpr double OnePi = 3.14159265358979323846;

		static constexpr double TwoPi = OnePi * 2.0;

		static constexpr double PiOverTwo = OnePi / 2.0;

		static constexpr double PiOverThree = OnePi / 3.0;

		// ���ֵת����ֵ �Ƚ�epsilon �Ĳ��
		template<glm::length_t L, typename T, glm::qualifier Q>
		static const glm::vec<L, T, Q> relativeEpsilonToAbsolute(
			const glm::vec<L, T, Q>& a,
			const glm::vec<L, T, Q>& b,
			double relativeEpsilon)
		{
			return relativeEpsilon * glm::max(glm::abs(a), glm::abs(b));
		}

		// ���ֵת����ֵ �Ƚ�epsilon �Ĳ�� double
		static constexpr double relativeEpsilonToAbsolute(
			double a,
			double b,
			double relativeEpsion ) noexcept {
			return relativeEpsion * glm::max(glm::abs(a), glm::abs(b));
		}
		
		// �������ֵ�Ƿ���ڸ����ľ��� epsilon
		template<glm::length_t L, typename T, glm::qualifier Q>
		static const glm::vec<L, T, Q> equalsEpsilon(
			const glm::vec<L, T, Q>& left,
			const glm::vec<L, T, Q>& right,
			double relativeEpsilon) noexcept
		{
			return PTMath::equalsEpsilon(left,right, relativeEpsilon, relativeEpsilon)��
		}

		// �������ֵ�Ƿ���ڸ����ľ��� epsilon double
		static constexpr bool equalsEpsilon(double left, double right, double relativeEpsilon, double absoluteEpsilon) noexcept 
		{
			const double diff = glm::abs(left - right);
			return diff <= absoluteEpsilon ||
				diff <= relativeEpsilonToAbsolute(left, right, relativeEpsilon);
		}

		// ʹ�þ���ֵ������Բ����ж�����ֵ�Ƿ���� dobule
		static constexpr bool equalsEpsilon(double left, double right,double relativeEpsilon) noexcept {
			return equalsEpsilon(left, right, relativeEpsilon, relativeEpsilon);
		}

		// ʹ�þ���ֵ������Բ����ж�����ֵ�Ƿ����
		template<glm::length_t L, typename T, glm::qualifier Q>
		static const glm::vec<L, T, Q> equalsEpsilon(
			const glm::vec<L, T, Q>& left,
			const glm::vec<L, T, Q>& right,
			double relativeEpsilon,
			double absoluteEpsilon) noexcept
		{
			const glm::vec<L, T, Q> diff = glm::abs(left - right);
			return glm::lessThanEqual(diff, glm::vec<L, T, Q>(absoluteEpsilon)) == glm::vec<T, bool, Q>(true) ||
				   glm::lessThanEqual(diff, relativeEpsilonToAbsolute(left, right, relativeEpsilon)) == glm::vec<L, bool, Q>(true);
		}

		// ����ֵ�ķ���
		static constexpr double sign(double value) noexcept {
			if (value == 0.0 || value != value) 
			{
				return value;
			}
			return value > 0 ? 1 : -1;
		}

		// ����ֵΪ���� ���� 1.0 Ϊ���� ���� -1.0
		static constexpr double signNotZero(double value) noexcept
		{
			return value < 0.0 ? -1.0 : 1.0;
		}

		// ȡ�����ĺ��� ---- ����Ҳ�ʺ�
		static double mod(double m, double n) noexcept 
		{
			if (PTMath::sign(m) == PTMath::sign(n) && glm::abs(m) < glm::abs(n))
			{
				return m;
			}
			return fmod(fmod(m, n) + n, n);
		}

		// ����һ�� 0�� 2Pi֮��Ļ���
		static double zeroToTwoPi(double angle) noexcept 
		{
			if (angle >= 0 && angle <= PTMath::TwoPi)
			{
				return angle;
			}
			const double mod = PTMath::mod(angle, PTMath::TwoPi);
			if (glm::abs(mod) < PTMath::Epsilon14 && glm::abs(angle) > PTMath::Epsilon14)
			{
				return PTMath::TwoPi;
			}

			return mod;
		}

		// ����һ��-Pi  �� Pi ֮��� ����
		static double negativePiToPi(double angle) noexcept
		{
			if (angle >= -PTMath::OnePi && angle <= PTMath::OnePi)
			{
				return angle;
			}
			return PTMath::zeroToTwoPi(angle + PTMath::OnePi) - PTMath::OnePi;
		}

		// �Ƕ�ת����
		static constexpr double degreesToRadians( double angleDegrees ) noexcept {
			return angleDegrees * PTMath::OnePi / 180.0;
		}
		// ����ת�Ƕ�
		static constexpr double radiansToDegrees(double angleDegrees) noexcept {
			return angleDegrees * 180.0 / PTMath::OnePi ;
		}

		// ��������ֵ֮������Բ�ֵ
		static double lerp(double p, double q, double time) noexcept
		{
			return glm::mix(p, q, time);
		}

		// ��һ��ֵ������ĳһ������
		static constexpr double clamp( double value,double min,double max ) noexcept {
			return glm::clamp(value, min, max);
		}

		// ��[ -1 , 1  ]��Χ��ֵת���� [ 0, range ]
		static double toSNorm(double value, double rangeMaximum = 255) noexcept {
			return glm::round(rangeMaximum * PTMath::clamp(value, -1.0, 1.0) * 0.5 + 0.5);
		}
		// ��[ 0, range ]��Χ��ֵת����[ -1 , 1  ] 
		static constexpr double fromSNorm(double value, double rangeMaximum = 255.0) noexcept {
			return (PTMath::clamp(value, 0.0, rangeMaximum) / rangeMaximum) * 2.0 - 1.0;
		}

		// ����ת���� ��Χ [ -Pi , Pi ]
		static double convertLongitudeRange(double angle) noexcept
		{
			const double towPi = PTMath::TwoPi;
			const double simplified = angle - glm::floor(angle / towPi) * TwoPi;

			if (simplified < -PTMath::OnePi) 
			{
				return simplified + towPi;
			}
			if (simplified >= PTMath::OnePi) 
			{
				return simplified + towPi;
			}

			return  simplified;
		}

		// �������� �ܽӽ���������ǲο� ceil �ӽ���С������ ���� floor ͨ����ֵ�ο�ȡ�����
		static double roundUp(double value, double tolerance) noexcept
		{
			const double up = glm::ceil(value);
			const double down = glm::floor(value);

			if (value - down < tolerance)
			{
				return down;
			}
			else {
				return up;
			}
		}

		// �������� �ܽӽ���������ǲο� ceil �ӽ���С������ ���� floor ͨ����ֵ�ο�ȡ��С��
		static double roundDown(double value, double tolerance) noexcept
		{
			const double up = glm::ceil(value);
			const double down = glm::floor(value);

			if (up - value < tolerance)
			{
				return value;
			}
			else {
				return down;
			}
		}
	};
}