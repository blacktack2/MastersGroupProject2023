/**
 * @file   Plane.h
 * @brief  Wrapper class for a 3D plane.
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   March 2023
 */
#pragma once
#include "Vector3.h"

namespace NCL {
	namespace Maths {
		/**
		 * @brief Wrapper class for a 3D plane.
		 */
		class Plane {
		public:
			Plane();
			Plane(const Vector3& normal, float distance, bool normalise = false);

			~Plane() = default;

			inline Plane& SetNormal(const Vector3& normal) {
				this->normal = normal;
				return *this;
			}
			inline Vector3 GetNormal() const {
				return normal;
			}

			inline Plane& SetDistance(float dist) {
				distance = dist;
				return *this;
			}
			inline float GetDistance() const {
				return distance;
			}

			bool SphereInPlane(const Vector3& position, float radius) const;
			bool PointInPlane(const Vector3& position) const;

			float DistanceFromPlane(const Vector3& in) const;

			Vector3 GetPointOnPlane() const {
				return normal * -distance;
			}

			Vector3 ProjectPointOntoPlane(const Vector3& point) const;

			static Plane PlaneFromTri(const Vector3& v0, const Vector3& v1, const Vector3& v2);
		protected:
			Vector3 normal;
			float distance;
		};
	}
}
