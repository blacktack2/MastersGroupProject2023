/**
 * @file   Maths.h
 * @brief  Utility file for general math functions and constants.
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   March 2023
 */
#pragma once
#include <algorithm>

namespace NCL {
	namespace Maths {
		class Vector2;
		class Vector3;

		static const float PI = 3.14159265358979323846f;
		static const float PI_OVER_360 = PI / 360.0f;

		inline float RadiansToDegrees(float rads) {
			return rads * 180.0f / PI;
		};

		inline float DegreesToRadians(float degs) {
			return degs * PI / 180.0f;
		};

		template<class T, typename std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
		inline T Clamp(T value, T min, T max) {
			return value < min ? min : (value > max ? max : value);
		}

		template<class T, typename std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
		inline T Lerp(T a, T b, float by) {
			return (a * (1.0f - by) + (b * by));
		}

		void ScreenBoxOfTri(const Vector3& v0, const Vector3& v1, const Vector3& v2, Vector2& topLeft, Vector2& bottomRight);

		int ScreenAreaOfTri(const Vector3& a, const Vector3& b, const Vector3& c);
		float FloatAreaOfTri(const Vector3& a, const Vector3& b, const Vector3& c);

		float CrossAreaOfTri(const Vector3& a, const Vector3& b, const Vector3& c);
	}
}