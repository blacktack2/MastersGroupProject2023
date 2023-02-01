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
	class Vector2;

	class Vector4 {

	public:
		union {
			struct {
				float x;
				float y;
				float z;
				float w;
			};
			float array[4];
		};

	public:
		constexpr Vector4(void) : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}

		explicit constexpr Vector4(float val) : x(val), y(val), z(val), w(val) {}

		constexpr Vector4(float xVal, float yVal, float zVal, float wVal) : x(xVal), y(yVal), z(zVal), w(wVal) {}

		Vector4(const Vector2& v2, float z, float w);
		Vector4(float x, const Vector2& v2, float w);
		Vector4(float x, float y, const Vector2& v2);
		Vector4(const Vector2& v2a, const Vector2& v2b);
		Vector4(const Vector3& v3, float w);
		Vector4(float x, const Vector3& v3);

		~Vector4(void) = default;

		Vector4 Normalised() const {
			Vector4 temp(*this);
			temp.Normalise();
			return temp;
		}

		void Normalise() {
			float length = Length();

			if (length != 0.0f) {
				length = 1.0f / length;
				x = x * length;
				y = y * length;
				z = z * length;
				w = w * length;
			}
		}

		inline float Length() const {
			return sqrt((x*x) + (y*y) + (z*z) + (w * w));
		}

		inline constexpr float LengthSquared() const {
			return ((x*x) + (y*y) + (z*z) + (w * w));
		}

		inline constexpr float GetMaxElement() const {
			return std::max(std::max(std::max(x, y), z), w);
		}

		inline constexpr float GetMinElement() const {
			return std::min(std::min(std::min(x, y), z), w);
		}

		float GetAbsMaxElement() const {
			return std::max(std::max(std::max(std::abs(x), std::abs(y)), std::abs(z)), std::abs(w));
		}

		float GetAbsMinElement() const {
			return std::min(std::min(std::min(std::abs(x), std::abs(y)), std::abs(z)), std::abs(w));
		}

		static inline constexpr Vector4 Clamp(const Vector4& input, const Vector4& mins, const Vector4& maxs) {
			return Vector4(
				std::clamp(input.x, mins.x, maxs.x),
				std::clamp(input.y, mins.y, maxs.y),
				std::clamp(input.z, mins.z, maxs.z),
				std::clamp(input.w, mins.w, maxs.w)
			);
		}

		static inline constexpr Vector4 Abs(const Vector4& input) {
			return Vector4(
				std::abs(input.x),
				std::abs(input.y),
				std::abs(input.z),
				std::abs(input.w)
			);
		}

		static inline constexpr float Dot(const Vector4 &a, const Vector4 &b) {
			return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
		}

		inline constexpr Vector4 operator+(const Vector4  &a) const {
			return Vector4(x + a.x, y + a.y, z + a.z, w + a.w);
		}

		inline constexpr Vector4 operator-(const Vector4  &a) const {
			return Vector4(x - a.x, y - a.y, z - a.z, w - a.w);
		}

		inline constexpr Vector4 operator-() const {
			return Vector4(-x, -y, -z, -w);
		}

		inline constexpr Vector4 operator*(float a)	const {
			return Vector4(x * a, y * a, z * a, w * a);
		}

		inline constexpr Vector4 operator*(const Vector4  &a) const {
			return Vector4(x * a.x, y * a.y, z * a.z, w * a.w);
		}

		inline constexpr Vector4 operator/(const Vector4  &a) const {
			return Vector4(x / a.x, y / a.y, z / a.z, w / a.w);
		};

		inline constexpr Vector4 operator/(float v) const {
			return Vector4(x / v, y / v, z / v, w / v);
		};

		inline constexpr void operator+=(const Vector4  &a) {
			x += a.x;
			y += a.y;
			z += a.z;
			w += a.w;
		}

		inline constexpr void operator-=(const Vector4  &a) {
			x -= a.x;
			y -= a.y;
			z -= a.z;
			w -= a.w;
		}


		inline constexpr void operator*=(const Vector4  &a) {
			x *= a.x;
			y *= a.y;
			z *= a.z;
			w *= a.w;
		}

		inline constexpr void operator/=(const Vector4  &a) {
			x /= a.x;
			y /= a.y;
			z /= a.z;
			w /= a.w;
		}

		inline constexpr void operator*=(float f) {
			x *= f;
			y *= f;
			z *= f;
			w *= f;
		}

		inline constexpr void operator/=(float f) {
			x /= f;
			y /= f;
			z /= f;
			w /= f;
		}

		inline constexpr float operator[](int i) const {
			return array[i];
		}

		inline constexpr float& operator[](int i) {
			return array[i];
		}

		inline constexpr bool operator==(const Vector4 &A)const {
			return (A.x == x && A.y == y && A.z == z && A.w == w);
		};

		inline constexpr bool operator!=(const Vector4 &A)const {
			return !(A.x == x && A.y == y && A.z == z && A.w == w);
		};

		inline friend std::ostream& operator<<(std::ostream& o, const Vector4& v) {
			o << "Vector4(" << v.x << "," << v.y << "," << v.z << "," << v.w << ")\n";
			return o;
		}
	};
}