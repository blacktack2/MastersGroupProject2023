#pragma once

#include "Camera.h"

#include "Transform.h"
#include "GameObject.h"

#include "AABBVolume.h"
#include "OBBVolume.h"
#include "SphereVolume.h"
#include "CapsuleVolume.h"
#include "Ray.h"

using NCL::Camera;
using namespace NCL::Maths;
using namespace NCL::CSC8503;
namespace NCL {
	class CollisionDetection
	{
	public:
		struct ContactPoint {
			Vector3 contactPoint;
			Vector3 normal;
			float	penetration;
		};

		struct CollisionInfo {
			GameObject* a;
			GameObject* b;		
			int framesLeft;
			bool isEntered;

			ContactPoint point;

			CollisionInfo() {

			}

			void AddContactPoint(const Vector3& contactPoint, const Vector3& normal, float p) {
				point.contactPoint  = contactPoint;
				point.normal		= normal;
				point.penetration	= p;
			}

			//Advanced collision detection / resolution
			bool operator < (const CollisionInfo& other) const {
				size_t otherHash = (size_t)other.a + ((size_t)other.b << 32);
				size_t thisHash  = (size_t)a + ((size_t)b << 32);

				if (thisHash < otherHash) {
					return true;
				}
				return false;
			}

			bool operator ==(const CollisionInfo& other) const {
				if (other.a == a && other.b == b) {
					return true;
				}
				return false;
			}
		};

		static void ClosestRayPoints(const Vector3& centerA, const Vector3& dirA, float lenA, const Vector3& centerB, const Vector3& dirB, float lenB,
			                         Vector3& bestA, Vector3& bestB);

		static bool RayBoxIntersection(const Ray&r, const Vector3& boxPos, const Vector3& boxSize, RayCollision& collision);

		static Ray BuildRayFromMouse(const Camera& c);
		static Ray BuildRayFromCamera(const Camera& c);

		static bool RayIntersection(const Ray&r, GameObject& object, RayCollision &collisions);


		static bool RayAABBIntersection(const Ray&r, const Transform& worldTransform, const AABBVolume&	volume, RayCollision& collision);
		static bool RayOBBIntersection(const Ray&r, const Transform& worldTransform, const OBBVolume&	volume, RayCollision& collision);
		static bool RaySphereIntersection(const Ray&r, const Transform& worldTransform, const SphereVolume& volume, RayCollision& collision);
		static bool RayCapsuleIntersection(const Ray& r, const Transform& worldTransform, const CapsuleVolume& volume, RayCollision& collision);

		static bool RayPlaneIntersection(const Ray&r, const Plane&p, RayCollision& collisions);

		static bool ObjectIntersection(GameObject* a, GameObject* b, CollisionInfo& collisionInfo);

		static bool AASquareTest(const Vector2& posA, const Vector2& posB, const Vector2& halfSizeA, const Vector2& halfSizeB);
		static bool	AABBTest(const Vector3& posA, const Vector3& posB, const Vector3& halfSizeA, const Vector3& halfSizeB);

		static bool SeparatingPlane(const Vector3& delta, const Vector3& plane, const Vector3* axes0, const Vector3& halfSize0, const Vector3* axes1, const Vector3& halfSize1);

		static bool AABBIntersection(	const AABBVolume& volumeA, const Transform& worldTransformA,
										const AABBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo);
		static bool OBBIntersection(	const OBBVolume& volumeA, const Transform& worldTransformA,
										const OBBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo);
		static bool SphereIntersection(	const SphereVolume& volumeA, const Transform& worldTransformA,
										const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo);
		static bool CapsuleIntersection(const CapsuleVolume& volumeA, const Transform& worldTransformA,
										const CapsuleVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo);
		
		static bool AABBOBBIntersection(      const AABBVolume& volumeA, const Transform& worldTransformA,
										      const OBBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo);
		static bool AABBSphereIntersection(   const AABBVolume& volumeA, const Transform& worldTransformA,
										      const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo);
		static bool AABBCapsuleIntersection(  const AABBVolume& volumeA, const Transform& worldTransformA,
										      const CapsuleVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo);

		static bool OBBSphereIntersection(    const OBBVolume& volumeA, const Transform& worldTransformA,
			                                  const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo);
		static bool OBBCapsuleIntersection(   const OBBVolume& volumeA, const Transform& worldTransformA,
			                                  const CapsuleVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo);

		static bool SphereCapsuleIntersection(const SphereVolume& volumeA, const Transform& worldTransformA,
			                                  const CapsuleVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo);


		static Vector3 Unproject(const Vector3& screenPos, const Camera& cam);

		static Vector3		UnprojectScreenPosition(Vector3 position, float aspect, float fov, const Camera &c);
		static Matrix4		GenerateInverseProjection(float aspect, float fov, float nearPlane, float farPlane);
		static Matrix4		GenerateInverseView(const Camera &c);

	protected:

	private:
		CollisionDetection()	{}
		~CollisionDetection()	{}
	};
}

