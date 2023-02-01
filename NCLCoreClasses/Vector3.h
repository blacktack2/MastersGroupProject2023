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
	class Vector2;
	class Vector4;

	class Vector3 {
	public:
		union {
			struct {
				float x;
				float y;
				float z;
			};
			float array[3];
		};
	public:
		constexpr Vector3(void) : x(0.0f), y(0.0f), z(0.0f) {}

		explicit constexpr Vector3(float val) : x(val), y(val), z(val) {}

		constexpr Vector3(float xVal, float yVal, float zVal) : x(xVal), y(yVal), z(zVal) {}

		Vector3(const Vector2& v2, float z);
		Vector3(const Vector4& v4);

		~Vector3(void) = default;

		Vector3 Normalised() const {
			Vector3 temp(*this);
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
			}
		}

		inline float Length() const {
			return std::sqrt((x*x) + (y*y) + (z*z));
		}

		inline constexpr float LengthSquared() const {
			return ((x*x) + (y*y) + (z*z));
		}

		inline constexpr float GetMaxElement() const {
			return std::max(std::max(x, y), z);
		}

		inline constexpr float GetMinElement() const {
			return std::min(std::min(x, y), z);
		}

		inline constexpr float GetAbsMaxElement() const {
			return std::max(std::max(std::abs(x), std::abs(y)), std::abs(z));
		}

		inline constexpr float GetAbsMinElement() const {
			return std::min(std::min(std::abs(x), std::abs(y)), std::abs(z));
		}

		static inline constexpr Vector3 Clamp(const Vector3& input, const Vector3& mins, const Vector3& maxs) {
			return Vector3(
				std::clamp(input.x, mins.x, maxs.x),
				std::clamp(input.y, mins.y, maxs.y),
				std::clamp(input.z, mins.z, maxs.z)
			);
		}

		static inline constexpr Vector3 Abs(const Vector3& input) {
			return Vector3(
				std::abs(input.x),
				std::abs(input.y),
				std::abs(input.z)
			);
		}

		static inline constexpr float Dot(const Vector3& a, const Vector3& b) {
			return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
		}

		static inline constexpr Vector3 Cross(const Vector3& a, const Vector3& b) {
			return Vector3((a.y * b.z) - (a.z * b.y), (a.z * b.x) - (a.x * b.z), (a.x * b.y) - (a.y * b.x));
		}

		static inline constexpr std::pair<Vector3, bool> CrossSAT(const Vector3& a, const Vector3& b) {
			Vector3 m = Cross(a.Normalised(), b.Normalised());
			if (m == Vector3(0)) {
				Vector3 n = Cross(a, b - a);
				m = Cross(a.Normalised(), n.Normalised());
				return std::make_pair(m, n != Vector3(0));
			} else {
				return std::make_pair(m, true);
			}
		}

		inline constexpr Vector3 operator+(const Vector3& a) const {
			return Vector3(x + a.x, y + a.y, z + a.z);
		}

		inline constexpr Vector3 operator-(const Vector3& a) const {
			return Vector3(x - a.x, y - a.y, z - a.z);
		}

		inline constexpr Vector3 operator-() const {
			return Vector3(-x, -y, -z);
		}

		inline constexpr Vector3 operator*(float a)	const {
			return Vector3(x * a, y * a, z * a);
		}

		inline constexpr Vector3 operator*(const Vector3& a) const {
			return Vector3(x * a.x, y * a.y, z * a.z);
		}

		inline constexpr Vector3 operator/(const Vector3& a) const {
			return Vector3(x / a.x, y / a.y, z / a.z);
		};

		inline constexpr Vector3 operator/(float v) const {
			return Vector3(x / v, y / v, z / v);
		};

		inline constexpr void operator+=(const Vector3& a) {
			x += a.x;
			y += a.y;
			z += a.z;
		}

		inline constexpr void operator-=(const Vector3& a) {
			x -= a.x;
			y -= a.y;
			z -= a.z;
		}


		inline constexpr void operator*=(const Vector3& a) {
			x *= a.x;
			y *= a.y;
			z *= a.z;
		}

		inline constexpr void operator/=(const Vector3& a) {
			x /= a.x;
			y /= a.y;
			z /= a.z;
		}

		inline constexpr void operator*=(float f) {
			x *= f;
			y *= f;
			z *= f;
		}

		inline constexpr void operator/=(float f) {
			x /= f;
			y /= f;
			z /= f;
		}

		inline constexpr float operator[](int i) const {
			return array[i];
		}

		inline constexpr float& operator[](int i) {
			return array[i];
		}

		inline constexpr bool operator==(const Vector3& A)const {
			return (A.x == x && A.y == y && A.z == z);
		};

		inline constexpr bool operator!=(const Vector3& A)const {
			return !(A.x == x && A.y == y && A.z == z);
		};

		inline friend std::ostream& operator<<(std::ostream& o, const Vector3& v) {
			o << "Vector3(" << v.x << "," << v.y << "," << v.z << ")\n";
			return o;
		}
	};
}