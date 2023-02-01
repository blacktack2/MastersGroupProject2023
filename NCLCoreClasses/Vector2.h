/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include <algorithm>
#include <cmath>
#include <iostream>

namespace NCL::Maths {
	class Vector3;
	class Vector4;
	class Vector2 {
	public:
		union {
			struct {
				float x;
				float y;
			};
			float array[2];
		};

	public:
		constexpr Vector2(void) : x(0.0f), y(0.0f) {}

		explicit constexpr Vector2(float val) : x(val), y(val) {}

		constexpr Vector2(float xVal, float yVal) : x(xVal), y(yVal) {}

		Vector2(const Vector3& v3);
		Vector2(const Vector4& v4);

		~Vector2(void) = default;

		Vector2 Normalised() const {
			Vector2 temp(*this);
			temp.Normalise();
			return temp;
		}

		void Normalise() {
			float length = Length();

			if (length != 0.0f) {
				length = 1.0f / length;
				x = x * length;
				y = y * length;
			}
		}

		inline float Length() const {
			return std::sqrt((x * x) + (y * y));
		}

		inline constexpr float LengthSquared() const {
			return ((x * x) + (y * y));
		}

		inline constexpr float GetMaxElement() const {
			return std::max(x, y);
		}

		inline constexpr float GetMinElement() const {
			return std::min(x, y);
		}

		inline constexpr float GetAbsMaxElement() const {
			return std::max(std::abs(x), std::abs(y));
		}

		inline constexpr float GetAbsMinElement() const {
			return std::min(std::abs(x), std::abs(y));
		}

		static inline constexpr Vector2 Clamp(const Vector2& input, const Vector2& mins, const Vector2& maxs) {
			return Vector2(
				std::clamp(input.x, mins.x, maxs.x),
				std::clamp(input.y, mins.y, maxs.y)
			);
		}

		static inline constexpr Vector2 Abs(const Vector2& input) {
			return Vector2(
				std::abs(input.x),
				std::abs(input.y)
			);
		}

		static inline constexpr float Dot(const Vector2& a, const Vector2& b) {
			return (a.x * b.x) + (a.y * b.y);
		}

		inline constexpr Vector2 operator+(const Vector2& a) const {
			return Vector2(x + a.x, y + a.y);
		}

		inline constexpr Vector2 operator-(const Vector2& a) const {
			return Vector2(x - a.x, y - a.y);
		}

		inline constexpr Vector2 operator-() const {
			return Vector2(-x, -y);
		}

		inline constexpr Vector2 operator*(float a)	const {
			return Vector2(x * a, y * a);
		}

		inline constexpr Vector2 operator*(const Vector2& a) const {
			return Vector2(x * a.x, y * a.y);
		}

		inline constexpr Vector2 operator/(const Vector2& a) const {
			return Vector2(x / a.x, y / a.y);
		};

		inline constexpr Vector2 operator/(float v) const {
			return Vector2(x / v, y / v);
		};

		inline constexpr void operator+=(const Vector2& a) {
			x += a.x;
			y += a.y;
		}

		inline constexpr void operator-=(const Vector2& a) {
			x -= a.x;
			y -= a.y;
		}

		inline constexpr void operator*=(const Vector2& a) {
			x *= a.x;
			y *= a.y;
		}

		inline constexpr void operator/=(const Vector2& a) {
			x /= a.x;
			y /= a.y;
		}

		inline constexpr void operator*=(float f) {
			x *= f;
			y *= f;
		}

		inline constexpr void operator/=(float f) {
			x /= f;
			y /= f;
		}

		inline constexpr float operator[](int i) const {
			return array[i];
		}

		inline constexpr float& operator[](int i) {
			return array[i];
		}

		inline constexpr bool operator==(const Vector2& A)const {
			return (A.x == x && A.y == y);
		};

		inline constexpr bool operator!=(const Vector2& A)const {
			return !(A.x == x && A.y == y);
		};

		inline friend std::ostream& operator<<(std::ostream& o, const Vector2& v) {
			o << "Vector2(" << v.x << "," << v.y << ")\n";
			return o;
		}
	};
}