#include "CollisionDetection.h"
#include "CollisionVolume.h"
#include "AABBVolume.h"
#include "OBBVolume.h"
#include "SphereVolume.h"
#include "Window.h"
#include "Maths.h"
#include "Debug.h"

using namespace NCL;

bool CollisionDetection::RayPlaneIntersection(const Ray&r, const Plane&p, RayCollision& collisions) {
	float ln = Vector3::Dot(p.GetNormal(), r.GetDirection());

	if (ln == 0.0f) {
		return false; //direction vectors are perpendicular!
	}
	
	Vector3 planePoint = p.GetPointOnPlane();

	Vector3 pointDir = planePoint - r.GetPosition();

	float d = Vector3::Dot(pointDir, p.GetNormal()) / ln;

	collisions.collidedAt = r.GetPosition() + (r.GetDirection() * d);

	return true;
}

bool CollisionDetection::RayIntersection(const Ray& r,GameObject& object, RayCollision& collision) {
	bool hasCollided = false;

	const Transform& worldTransform = object.GetTransform();
	const CollisionVolume* volume	= object.GetBoundingVolume();

	if (!volume || !DoCollisionLayersOverlap(r.GetLayer(), volume->layer)) {
		return false;
	}

	switch (volume->type) {
		case VolumeType::AABB:		hasCollided = RayAABBIntersection(r, worldTransform, (const AABBVolume&)*volume	, collision); break;
		case VolumeType::OBB:		hasCollided = RayOBBIntersection(r, worldTransform, (const OBBVolume&)*volume	, collision); break;
		case VolumeType::Sphere:	hasCollided = RaySphereIntersection(r, worldTransform, (const SphereVolume&)*volume	, collision); break;

		case VolumeType::Capsule:	hasCollided = RayCapsuleIntersection(r, worldTransform, (const CapsuleVolume&)*volume, collision); break;
	}

	return hasCollided;
}

void NCL::CollisionDetection::ClosestRayPoints(const Vector3& centerA, const Vector3& dirA, float lenA,
	const Vector3& centerB, const Vector3& dirB, float lenB,
	Vector3& bestA, Vector3& bestB) {
	float t1, t2;
	if (dirA == dirB) {
		t1 = 0;
		t2 = 0;
	} else {
		Vector3 n = -Vector3::Cross(dirA, dirB);

		t1 = Vector3::Dot(Vector3::Cross(dirB, n), (centerA - centerB)) / n.LengthSquared();
		t2 = Vector3::Dot(Vector3::Cross(dirA, n), (centerA - centerB)) / n.LengthSquared();
	}

	bool clampedA = false;
	bool clampedB = false;
	if (lenA != 0) {
		float old = t1;
		t1 = std::clamp(t1, -lenA, lenA);
		clampedA = t1 != old;
	}
	if (lenB != 0) {
		float old = t2;
		t2 = std::clamp(t2, -lenB, lenB);
		clampedB = t2 != old;
	}

	if (clampedA && !clampedB) {
		bestA = centerA + dirA * t1;

		float proj = Vector3::Dot(bestA - centerB, dirB);
		bestB = centerB + dirB * proj;
	} else if (clampedB && !clampedA) {
		bestB = centerB + dirB * t2;

		float proj = Vector3::Dot(bestB - centerA, dirA);
		bestA = centerA + dirA * proj;
	} else {
		bestA = centerA + dirA * t1;
		bestB = centerB + dirB * t2;
	}
}

bool CollisionDetection::RayBoxIntersection(const Ray&r, const Vector3& boxPos, const Vector3& boxSize, RayCollision& collision) {
	Vector3 boxMin = boxPos - boxSize;
	Vector3 boxMax = boxPos + boxSize;

	Vector3 rayPos = r.GetPosition();
	Vector3 rayDir = r.GetDirection();

	Vector3 tVals = Vector3(-1, -1, -1);

	for (int i = 0; i < 3; i++) {
		if (rayDir[i] > 0) {
			tVals[i] = (boxMin[i] - rayPos[i]) / rayDir[i];
		} else if (rayDir[i] < 0) {
			tVals[i] = (boxMax[i] - rayPos[i]) / rayDir[i];
		}
	}

	float bestT = tVals.GetMaxElement();
	if (bestT < 0.0f) {
		return false;
	}

	Vector3 intersection = rayPos + (rayDir * bestT);
	const float epsilon = 0.0001f;
	for (int i = 0; i < 3; i++) {
		if (intersection[i] + epsilon < boxMin[i] || intersection[i] - epsilon > boxMax[i]) {
			return false;
		}
	}

	collision.collidedAt = intersection;
	collision.rayDistance = bestT;
	return true;
}

bool CollisionDetection::RayAABBIntersection(const Ray&r, const Transform& worldTransform, const AABBVolume& volume, RayCollision& collision) {
	Vector3 boxPos = worldTransform.GetPosition();
	Vector3 boxSize = volume.GetHalfDimensions();
	return RayBoxIntersection(r, boxPos, boxSize, collision);
}

bool CollisionDetection::RayOBBIntersection(const Ray&r, const Transform& worldTransform, const OBBVolume& volume, RayCollision& collision) {
	Quaternion orientation = worldTransform.GetOrientation();
	Vector3 position = worldTransform.GetPosition();

	Matrix3 transform = Matrix3(orientation);
	Matrix3 invTransform = Matrix3(orientation.Conjugate());

	Vector3 localRayPos = r.GetPosition() - position;

	Ray tempRay = Ray(invTransform * localRayPos, invTransform * r.GetDirection());

	if (RayBoxIntersection(tempRay, Vector3(), volume.GetHalfDimensions(), collision)) {
		collision.collidedAt = (transform * collision.collidedAt) + position;
		return true;
	}
	return false;
}

bool CollisionDetection::RaySphereIntersection(const Ray&r, const Transform& worldTransform, const SphereVolume& volume, RayCollision& collision) {
	Vector3 spherePos = worldTransform.GetPosition();
	float sphereRadius = volume.GetRadius();

	Vector3 dir = spherePos - r.GetPosition();

	float sphereProj = Vector3::Dot(dir, r.GetDirection());

	if (sphereProj < 0.0f) {
		return false;
	}

	Vector3 point = r.GetPosition() + (r.GetDirection() * sphereProj);

	float sphereDist = (point - spherePos).Length();

	if (sphereDist > sphereRadius) {
		return false;
	}

	float offset = sqrt((sphereRadius * sphereRadius) - (sphereDist * sphereDist));

	collision.rayDistance = sphereProj - offset;
	collision.collidedAt = r.GetPosition() + (r.GetDirection() * collision.rayDistance);

	return true;
}

bool CollisionDetection::RayCapsuleIntersection(const Ray& r, const Transform& worldTransform, const CapsuleVolume& volume, RayCollision& collision) {
	Vector3 position = worldTransform.GetPosition();
	Quaternion orientation = worldTransform.GetOrientation();
	float radius = volume.GetRadius() * 0.5f;
	float halfHeight = volume.GetHalfHeight();

	Vector3 capsuleDir = orientation * Vector3(0, 1, 0);

	Vector3 capsulePoint;
	Vector3 rayPoint;
	ClosestRayPoints(position, capsuleDir, halfHeight - radius, r.GetPosition(), r.GetDirection(), 0.0f, capsulePoint, rayPoint);

	float distanceSquared = (rayPoint - capsulePoint).LengthSquared();
	if (distanceSquared > radius * radius) {
		return false;
	}

	collision.rayDistance = (rayPoint - r.GetPosition()).Length();
	collision.collidedAt = rayPoint;
	return true;
}

bool CollisionDetection::ObjectIntersection(GameObject* a, GameObject* b, CollisionInfo& collisionInfo) {
	const CollisionVolume* volA = a->GetBoundingVolume();
	const CollisionVolume* volB = b->GetBoundingVolume();

	if (!volA || !volB || !DoCollisionLayersOverlap(volA->layer, volB->layer)) {
		return false;
	}

	collisionInfo.a = a;
	collisionInfo.b = b;

	Transform& transformA = a->GetTransform();
	Transform& transformB = b->GetTransform();

	int pairType = (int)volA->type | (int)volB->type;

	if (pairType == (int)VolumeType::AABB) {
		return AABBIntersection((AABBVolume&)*volA, transformA, (AABBVolume&)*volB, transformB, collisionInfo);
	}
	if (pairType == (int)VolumeType::OBB) {
		return OBBIntersection((OBBVolume&)*volA, transformA, (OBBVolume&)*volB, transformB, collisionInfo);
	}
	if (pairType == (int)VolumeType::Sphere) {
		return SphereIntersection((SphereVolume&)*volA, transformA, (SphereVolume&)*volB, transformB, collisionInfo);
	}
	if (pairType == (int)VolumeType::Capsule) {
		return CapsuleIntersection((CapsuleVolume&)* volA, transformA, (CapsuleVolume&)* volB, transformB, collisionInfo);
	}

	if (pairType == ((int)VolumeType::AABB | (int)VolumeType::OBB)) {
		if (volB->type == VolumeType::AABB) {
			collisionInfo.a = b;
			collisionInfo.b = a;
			return AABBOBBIntersection((AABBVolume&)*volB, transformB, (OBBVolume&)*volA, transformA, collisionInfo);
		} else {
			return AABBOBBIntersection((AABBVolume&)*volA, transformA, (OBBVolume&)*volB, transformB, collisionInfo);
		}
	}
	if (pairType == ((int)VolumeType::AABB | (int)VolumeType::Sphere)) {
		if (volB->type == VolumeType::AABB) {
			collisionInfo.a = b;
			collisionInfo.b = a;
			return AABBSphereIntersection((AABBVolume&)*volB, transformB, (SphereVolume&)*volA, transformA, collisionInfo);
		} else {
			return AABBSphereIntersection((AABBVolume&)*volA, transformA, (SphereVolume&)*volB, transformB, collisionInfo);
		}
	}
	if (pairType == ((int)VolumeType::AABB | (int)VolumeType::Capsule)) {
		if (volB->type == VolumeType::AABB) {
			collisionInfo.a = b;
			collisionInfo.b = a;
			return AABBCapsuleIntersection((AABBVolume&)*volB, transformB, (CapsuleVolume&)*volA, transformA, collisionInfo);
		} else {
			return AABBCapsuleIntersection((AABBVolume&)*volA, transformA, (CapsuleVolume&)*volB, transformB, collisionInfo);
		}
	}

	if (pairType == ((int)VolumeType::OBB | (int)VolumeType::Sphere)) {
		if (volB->type == VolumeType::OBB) {
			collisionInfo.a = b;
			collisionInfo.b = a;
			return OBBSphereIntersection((OBBVolume&)*volB, transformB, (SphereVolume&)*volA, transformA, collisionInfo);
		} else {
			return OBBSphereIntersection((OBBVolume&)*volA, transformA, (SphereVolume&)*volB, transformB, collisionInfo);
		}
	}
	if (pairType == ((int)VolumeType::OBB | (int)VolumeType::Capsule)) {
		if (volB->type == VolumeType::OBB) {
			collisionInfo.a = b;
			collisionInfo.b = a;
			return OBBCapsuleIntersection((OBBVolume&)*volB, transformB, (CapsuleVolume&)*volA, transformA, collisionInfo);
		} else {
			return OBBCapsuleIntersection((OBBVolume&)*volA, transformA, (CapsuleVolume&)*volB, transformB, collisionInfo);
		}
	}

	if (pairType == ((int)VolumeType::Sphere | (int)VolumeType::Capsule)) {
		if (volB->type == VolumeType::Sphere) {
			collisionInfo.a = b;
			collisionInfo.b = a;
			return SphereCapsuleIntersection((SphereVolume&)*volB, transformB, (CapsuleVolume&)*volA, transformA, collisionInfo);
		} else {
			return SphereCapsuleIntersection((SphereVolume&)*volA, transformA, (CapsuleVolume&)*volB, transformB, collisionInfo);
		}
	}

	return false;
}

bool NCL::CollisionDetection::AASquareTest(const Vector2& posA, const Vector2& posB, const Vector2& halfSizeA, const Vector2& halfSizeB) {
	Vector2 delta = posB - posA;
	Vector2 totalSize = halfSizeA + halfSizeB;

	return (std::abs(delta.x) < totalSize.x && std::abs(delta.y) < totalSize.y);
}

bool CollisionDetection::AABBTest(const Vector3& posA, const Vector3& posB, const Vector3& halfSizeA, const Vector3& halfSizeB) {
	Vector3 delta = posB - posA;
	Vector3 totalSize = halfSizeA + halfSizeB;

	return (std::abs(delta.x) < totalSize.x && std::abs(delta.y) < totalSize.y && std::abs(delta.z) < totalSize.z);
}

bool CollisionDetection::AABBIntersection(const AABBVolume& volumeA, const Transform& worldTransformA,
	const AABBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	Vector3 boxAPos = worldTransformA.GetPosition();
	Vector3 boxBPos = worldTransformB.GetPosition();

	Vector3 boxASize = volumeA.GetHalfDimensions();
	Vector3 boxBSize = volumeB.GetHalfDimensions();

	if (AABBTest(boxAPos, boxBPos, boxASize, boxBSize)) {
		static const Vector3 faces[6] = {
			Vector3(-1,  0,  0), Vector3(1,  0,  0),
			Vector3(0, -1,  0), Vector3(0,  1,  0),
			Vector3(0,  0, -1), Vector3(0,  0,  1),
		};

		Vector3 maxA = boxAPos + boxASize;
		Vector3 minA = boxAPos - boxASize;

		Vector3 maxB = boxBPos + boxBSize;
		Vector3 minB = boxBPos - boxBSize;

		float distances[6] = {
			(maxB.x - minA.x),
			(maxA.x - minB.x),
			(maxB.y - minA.y),
			(maxA.y - minB.y),
			(maxB.z - minA.z),
			(maxA.z - minB.z)
		};

		float penetration = FLT_MAX;
		Vector3 bestAxis;

		for (int i = 0; i < 6; i++) {
			if (distances[i] < penetration) {
				penetration = distances[i];
				bestAxis = faces[i];
			}
		}

		collisionInfo.AddContactPoint(Vector3(), bestAxis, penetration);
		return true;
	}
	return false;
}

bool CollisionDetection::OBBIntersection(const OBBVolume& volumeA, const Transform& worldTransformA,
	const OBBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {

	Vector3 posA = worldTransformA.GetPosition();
	Vector3 halfSizeA = volumeA.GetHalfDimensions();
	Quaternion orientationA = worldTransformA.GetOrientation();

	Matrix3 orientationMatA = Matrix3(orientationA);

	Vector3 posB = worldTransformB.GetPosition();
	Vector3 halfSizeB = volumeB.GetHalfDimensions();
	Quaternion orientationB = worldTransformB.GetOrientation();

	Matrix3 orientationMatB = Matrix3(orientationB);

	Vector3 delta = posB - posA;

	Vector3 axesA[] = {
		orientationMatA.GetColumn(0),
		orientationMatA.GetColumn(1),
		orientationMatA.GetColumn(2)
	};
	Vector3 axesB[] = {
		orientationMatB.GetColumn(0),
		orientationMatB.GetColumn(1),
		orientationMatB.GetColumn(2)
	};

	for (int i = 0; i < 3; i++) {
		if (SeparatingPlane(delta, axesA[i], axesA, halfSizeA, axesB, halfSizeB) ||
			SeparatingPlane(delta, axesB[i], axesA, halfSizeA, axesB, halfSizeB)) {
			return false;
		}
		for (int j = 0; j < 3; j++) {
			std::pair<Vector3, bool> cross = Vector3::CrossSAT(axesA[i], axesB[j]);
			if (cross.second && SeparatingPlane(delta, cross.first, axesA, halfSizeA, axesB, halfSizeB)) {
				return false;
			}
		}
	}

	collisionInfo.AddContactPoint(posB, Vector3(0, 1, 0), 0.1f);
	return true;
}

bool CollisionDetection::SphereIntersection(const SphereVolume& volumeA, const Transform& worldTransformA,
	const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	float radii = volumeA.GetRadius() + volumeB.GetRadius();
	Vector3 delta = worldTransformB.GetPosition() - worldTransformA.GetPosition();

	float deltaLength = delta.Length();

	if (deltaLength < radii) {
		float penetration = radii - deltaLength;
		Vector3 normal = delta.Normalised();
		Vector3 collisionPoint = worldTransformA.GetPosition() + (normal * (volumeA.GetRadius() - (penetration * 0.5f)));

		collisionInfo.AddContactPoint(collisionPoint, normal, penetration);
		return true;
	}
	return false;
}

bool NCL::CollisionDetection::CapsuleIntersection(const CapsuleVolume& volumeA, const Transform& worldTransformA,
	const CapsuleVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	Vector3 centerA = worldTransformA.GetPosition();
	Vector3 centerB = worldTransformB.GetPosition();
	Quaternion orientationA = worldTransformA.GetOrientation();
	Quaternion orientationB = worldTransformB.GetOrientation();
	float radiusA = volumeA.GetRadius() * 0.5f;
	float radiusB = volumeB.GetRadius() * 0.5f;
	float halfHeightA = volumeA.GetHalfHeight();
	float halfHeightB = volumeB.GetHalfHeight();

	float offsetA = halfHeightA - radiusA;
	float offsetB = halfHeightB - radiusB;
	Vector3 dirA = orientationA * Vector3(0, 1, 0);
	Vector3 dirB = orientationB * Vector3(0, 1, 0);

	Vector3 pointA;
	Vector3 pointB;
	ClosestRayPoints(centerA, dirA, offsetA, centerB, dirB, offsetB, pointA, pointB);

	float distanceSquared = (pointA - pointB).LengthSquared();
	float radii = radiusA + radiusB;
	if (distanceSquared > radii * radii) {
		return false;
	}

	float penetration = radii - std::sqrt(distanceSquared);
	Vector3 normal = (pointB - pointA).Normalised();
	Vector3 collisionPoint = centerA + (normal * (radiusA - (penetration * 0.5f)));

	collisionInfo.AddContactPoint(collisionPoint, normal, penetration);
	return true;
}

bool NCL::CollisionDetection::SeparatingPlane(const Vector3& delta, const Vector3& plane, const Vector3* axes0, const Vector3& halfSize0, const Vector3* axes1, const Vector3& halfSize1) {
	float t = std::abs(Vector3::Dot(delta, plane));
	float v = 0;
	for (int i = 0; i < 3; i++) {
		v += std::abs(Vector3::Dot(axes0[i], plane) * halfSize0[i]);
		v += std::abs(Vector3::Dot(axes1[i], plane) * halfSize1[i]);
	}
	return t > v;
}

bool NCL::CollisionDetection::AABBOBBIntersection(const AABBVolume& volumeA, const Transform& worldTransformA,
	const OBBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	Vector3 posA = worldTransformA.GetPosition();
	Vector3 halfSizeA = volumeA.GetHalfDimensions();

	Vector3 posB = worldTransformB.GetPosition();
	Vector3 halfSizeB = volumeB.GetHalfDimensions();
	Quaternion orientationB = worldTransformB.GetOrientation();

	Matrix3 orientationMatB = Matrix3(orientationB);

	Vector3 delta = posB - posA;

	Vector3 axesA[] = {
		Vector3(1, 0, 0),
		Vector3(0, 1, 0),
		Vector3(0, 0, 1)
	};
	Vector3 axesB[] = {
		orientationMatB.GetColumn(0),
		orientationMatB.GetColumn(1),
		orientationMatB.GetColumn(2)
	};

	for (int i = 0; i < 3; i++) {
		if (SeparatingPlane(delta, axesA[i], axesA, halfSizeA, axesB, halfSizeB) ||
			SeparatingPlane(delta, axesB[i], axesA, halfSizeA, axesB, halfSizeB)) {
			return false;
		}
		for (int j = 0; j < 3; j++) {
			if (SeparatingPlane(delta, Vector3::Cross(axesA[i], axesB[j]), axesA, halfSizeA, axesB, halfSizeB)) {
				return false;
			}
		}
	}

	collisionInfo.AddContactPoint(posB, Vector3(0, 1, 0), 0.1f);
	return true;
}

bool CollisionDetection::AABBSphereIntersection(const AABBVolume& volumeA, const Transform& worldTransformA,
	const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	Vector3 boxSize = volumeA.GetHalfDimensions();

	Vector3 delta = worldTransformB.GetPosition() - worldTransformA.GetPosition();

	Vector3 closestPointOnBox = Maths::Clamp(delta, -boxSize, boxSize);

	Vector3 localPoint = delta - closestPointOnBox;
	float distance = localPoint.Length();

	if (distance < volumeB.GetRadius()) {
		Vector3 normal = distance == 0 ? delta.Normalised() : localPoint.Normalised();
		float penetration = volumeB.GetRadius() - distance;

		collisionInfo.AddContactPoint(worldTransformA.GetPosition() + closestPointOnBox, normal, penetration);
		return true;
	}
	return false;
}

bool NCL::CollisionDetection::AABBCapsuleIntersection(const AABBVolume& volumeA, const Transform& worldTransformA,
	const CapsuleVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	Vector3 boxPos = worldTransformA.GetPosition();
	Vector3 boxSize = volumeA.GetHalfDimensions();

	Vector3 capsulePos = worldTransformB.GetPosition();
	Vector3 capsuleDir = worldTransformB.GetOrientation() * Vector3(0, 1, 0);
	float capsuleRadius = volumeB.GetRadius() * 0.5f;
	float capsuleHalfHeight = volumeB.GetHalfHeight();

	float capsuleOffset = capsuleHalfHeight - capsuleRadius;
	Vector3 capsuleHead = capsulePos + capsuleDir * capsuleOffset;
	Vector3 capsuleTail = capsulePos - capsuleDir * capsuleOffset;

	Vector3 headDelta = capsuleHead - boxPos;
	Vector3 closestPointToHead = Maths::Clamp(headDelta, -boxSize, boxSize);
	Vector3 localPointHead = headDelta - closestPointToHead;
	float headDistanceSquared = localPointHead.LengthSquared();

	Vector3 tailDelta = capsuleTail - boxPos;
	Vector3 closestPointToTail = Maths::Clamp(tailDelta, -boxSize, boxSize);
	Vector3 localPointTail = tailDelta - closestPointToTail;
	float tailDistanceSquared = localPointTail.LengthSquared();

	Vector3 boxPoint = boxPos + (headDistanceSquared < tailDistanceSquared ? closestPointToHead : closestPointToTail);

	float proj = std::clamp(Vector3::Dot(boxPoint - capsulePos, capsuleDir), -capsuleOffset, capsuleOffset);
	Vector3 capsulePoint = capsulePos + capsuleDir * proj;

	float distanceSquared = (capsulePoint - boxPoint).LengthSquared();
	if (distanceSquared > capsuleRadius * capsuleRadius) {
		return false;
	}

	float penetration = capsuleRadius - std::sqrt(distanceSquared);
	Vector3 normal = (capsulePoint - boxPoint).Normalised();

	collisionInfo.AddContactPoint(boxPoint, normal, penetration);
	return true;
}

bool  CollisionDetection::OBBSphereIntersection(const OBBVolume& volumeA, const Transform& worldTransformA,
	const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	Vector3 boxSize = volumeA.GetHalfDimensions();
	Quaternion orientation = worldTransformA.GetOrientation();

	Vector3 obbPos = worldTransformA.GetPosition();
	Vector3 spherePos = orientation.Conjugate() * (worldTransformB.GetPosition() - obbPos);

	Vector3 closestPointOnBox = Maths::Clamp(spherePos, -boxSize, boxSize);

	Vector3 localPoint = spherePos - closestPointOnBox;
	float distanceSquared = localPoint.LengthSquared();

	if (distanceSquared < volumeB.GetRadius() * volumeB.GetRadius()) {
		Vector3 normal = distanceSquared == 0 ? spherePos.Normalised() : localPoint.Normalised();
		float penetration = volumeB.GetRadius() - std::sqrt(distanceSquared);

		collisionInfo.AddContactPoint(obbPos + orientation * closestPointOnBox, orientation * normal, penetration);
		return true;
	}

	return false;
}

bool NCL::CollisionDetection::OBBCapsuleIntersection(const OBBVolume& volumeA, const Transform& worldTransformA,
	const CapsuleVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	Vector3 boxPos = worldTransformA.GetPosition();
	Vector3 boxSize = volumeA.GetHalfDimensions();
	Quaternion boxOrientation = worldTransformA.GetOrientation();
	Quaternion boxOrientationInv = boxOrientation.Conjugate();

	Vector3 capsulePos = boxOrientationInv * (worldTransformB.GetPosition() - boxPos);
	Vector3 capsuleDir = boxOrientationInv * worldTransformB.GetOrientation() * Vector3(0, 1, 0);
	float capsuleRadius = volumeB.GetRadius() * 0.5f;
	float capsuleHalfHeight = volumeB.GetHalfHeight();

	float capsuleOffset = capsuleHalfHeight - capsuleRadius;
	Vector3 capsuleHead = capsulePos + capsuleDir * capsuleOffset;
	Vector3 capsuleTail = capsulePos - capsuleDir * capsuleOffset;

	Vector3 closestPointToHead = Maths::Clamp(capsuleHead, -boxSize, boxSize);
	Vector3 localPointHead = capsuleHead - closestPointToHead;
	float headDistanceSquared = localPointHead.LengthSquared();

	Vector3 closestPointToTail = Maths::Clamp(capsuleTail, -boxSize, boxSize);
	Vector3 localPointTail = capsuleTail - closestPointToTail;
	float tailDistanceSquared = localPointTail.LengthSquared();

	Vector3 boxPoint = headDistanceSquared < tailDistanceSquared ? closestPointToHead : closestPointToTail;

	float proj = std::clamp(Vector3::Dot(boxPoint - capsulePos, capsuleDir), -capsuleOffset, capsuleOffset);
	Vector3 capsulePoint = capsulePos + capsuleDir * proj;

	float distanceSquared = (capsulePoint - boxPoint).LengthSquared();
	if (distanceSquared > capsuleRadius * capsuleRadius) {
		return false;
	}

	float penetration = capsuleRadius - std::sqrt(distanceSquared);
	Vector3 normal = (capsulePoint - boxPoint).Normalised();

	collisionInfo.AddContactPoint(boxPos + boxOrientation * boxPoint, boxOrientation * normal, penetration);
	return true;
}

bool NCL::CollisionDetection::SphereCapsuleIntersection(const SphereVolume& volumeA, const Transform& worldTransformA,
	const CapsuleVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	Vector3 sphereCenter = worldTransformA.GetPosition();
	float sphereRadius = volumeA.GetRadius();

	Vector3 capsuleCenter = worldTransformB.GetPosition();
	Quaternion capsuleOrientation = worldTransformB.GetOrientation();
	float capsuleRadius = volumeB.GetRadius() * 0.5f;
	float capsuleHalfHeight = volumeB.GetHalfHeight();
	Vector3 capsuleDir = capsuleOrientation * Vector3(0, 1, 0);

	float proj = std::clamp(Vector3::Dot(sphereCenter - capsuleCenter, capsuleDir), -capsuleHalfHeight, capsuleHalfHeight);
	Vector3 capsulePos = capsuleCenter + capsuleDir * proj;

	float distanceSquared = (capsulePos - sphereCenter).LengthSquared();
	float radii = sphereRadius + capsuleRadius;
	if (distanceSquared > radii * radii) {
		return false;
	}

	float penetration = radii - std::sqrt(distanceSquared);
	Vector3 normal = (capsulePos - sphereCenter).Normalised();
	Vector3 collisionPoint = sphereCenter + (normal * (sphereRadius - (penetration * 0.5f)));

	collisionInfo.AddContactPoint(collisionPoint, normal, penetration);
	return true;
}

Matrix4 GenerateInverseView(const Camera &c) {
	float pitch = c.GetPitch();
	float yaw	= c.GetYaw();
	Vector3 position = c.GetPosition();

	Matrix4 iview =
		Matrix4::Translation(position) *
		Matrix4::Rotation(-yaw, Vector3(0, -1, 0)) *
		Matrix4::Rotation(-pitch, Vector3(-1, 0, 0));

	return iview;
}

Matrix4 GenerateInverseProjection(float aspect, float nearPlane, float farPlane, float fov) {
	float negDepth = nearPlane - farPlane;

	float invNegDepth = negDepth / (2 * (farPlane * nearPlane));

	Matrix4 m;

	float h = 1.0f / tan(fov*PI_OVER_360);

	m.array[0][0] = aspect / h;
	m.array[1][1] = tan(fov * PI_OVER_360);
	m.array[2][2] = 0.0f;

	m.array[2][3] = invNegDepth;//// +PI_OVER_360;
	m.array[3][2] = -1.0f;
	m.array[3][3] = (0.5f / nearPlane) + (0.5f / farPlane);

	return m;
}

Vector3 CollisionDetection::Unproject(const Vector3& screenPos, const Camera& cam) {
	Vector2 screenSize = Window::GetWindow()->GetScreenSize();

	float aspect	= screenSize.x / screenSize.y;
	float fov		= cam.GetFieldOfVision();
	float nearPlane = cam.GetNearPlane();
	float farPlane  = cam.GetFarPlane();

	//Create our inverted matrix! Note how that to get a correct inverse matrix,
	//the order of matrices used to form it are inverted, too.
	Matrix4 invVP = GenerateInverseView(cam) * GenerateInverseProjection(aspect, fov, nearPlane, farPlane);

	Matrix4 proj  = cam.BuildProjectionMatrix(aspect);

	//Our mouse position x and y values are in 0 to screen dimensions range,
	//so we need to turn them into the -1 to 1 axis range of clip space.
	//We can do that by dividing the mouse values by the width and height of the
	//screen (giving us a range of 0.0 to 1.0), multiplying by 2 (0.0 to 2.0)
	//and then subtracting 1 (-1.0 to 1.0).
	Vector4 clipSpace = Vector4(
		(screenPos.x / (float)screenSize.x) * 2.0f - 1.0f,
		(screenPos.y / (float)screenSize.y) * 2.0f - 1.0f,
		(screenPos.z),
		1.0f
	);

	//Then, we multiply our clipspace coordinate by our inverted matrix
	Vector4 transformed = invVP * clipSpace;

	//our transformed w coordinate is now the 'inverse' perspective divide, so
	//we can reconstruct the final world space by dividing x,y,and z by w.
	return Vector3(transformed.x / transformed.w, transformed.y / transformed.w, transformed.z / transformed.w);
}

Ray CollisionDetection::BuildRayFromMouse(const Camera& cam) {
	Vector2 screenMouse = Window::GetMouse()->GetAbsolutePosition();
	Vector2 screenSize	= Window::GetWindow()->GetScreenSize();

	//We remove the y axis mouse position from height as OpenGL is 'upside down',
	//and thinks the bottom left is the origin, instead of the top left!
	Vector3 nearPos = Vector3(screenMouse.x,
		screenSize.y - screenMouse.y,
		-0.99999f
	);

	//We also don't use exactly 1.0 (the normalised 'end' of the far plane) as this
	//causes the unproject function to go a bit weird. 
	Vector3 farPos = Vector3(screenMouse.x,
		screenSize.y - screenMouse.y,
		0.99999f
	);

	Vector3 a = Unproject(nearPos, cam);
	Vector3 b = Unproject(farPos, cam);
	Vector3 c = b - a;

	c.Normalise();

	return Ray(cam.GetPosition(), c);
}

Ray CollisionDetection::BuildRayFromCamera(const Camera& cam) {
	Vector2 screenSize = Window::GetWindow()->GetScreenSize();

	Vector3 nearPos = Vector3(screenSize * 0.5f, -0.99999f);
	Vector3 farPos  = Vector3(screenSize * 0.5f,  0.99999f);

	Vector3 a = Unproject(nearPos, cam);
	Vector3 b = Unproject(farPos , cam);
	Vector3 c = b - a;

	c.Normalise();

	return Ray(cam.GetPosition(), c);
}

//http://bookofhook.com/mousepick.pdf
Matrix4 CollisionDetection::GenerateInverseProjection(float aspect, float fov, float nearPlane, float farPlane) {
	Matrix4 m;

	float t = tan(fov*PI_OVER_360);

	float neg_depth = nearPlane - farPlane;

	const float h = 1.0f / t;

	float c = (farPlane + nearPlane) / neg_depth;
	float e = -1.0f;
	float d = 2.0f*(nearPlane*farPlane) / neg_depth;

	m.array[0][0] = aspect / h;
	m.array[1][1] = tan(fov * PI_OVER_360);
	m.array[2][2] = 0.0f;

	m.array[2][3] = 1.0f / d;

	m.array[3][2] = 1.0f / e;
	m.array[3][3] = -c / (d * e);

	return m;
}

/*
And here's how we generate an inverse view matrix. It's pretty much
an exact inversion of the BuildViewMatrix function of the Camera class!
*/
Matrix4 CollisionDetection::GenerateInverseView(const Camera &c) {
	float pitch = c.GetPitch();
	float yaw	= c.GetYaw();
	Vector3 position = c.GetPosition();

	Matrix4 iview =
		Matrix4::Translation(position) *
		Matrix4::Rotation(yaw, Vector3(0, 1, 0)) *
		Matrix4::Rotation(pitch, Vector3(1, 0, 0));

	return iview;
}


/*
If you've read through the Deferred Rendering tutorial you should have a pretty
good idea what this function does. It takes a 2D position, such as the mouse
position, and 'unprojects' it, to generate a 3D world space position for it.

Just as we turn a world space position into a clip space position by multiplying
it by the model, view, and projection matrices, we can turn a clip space
position back to a 3D position by multiply it by the INVERSE of the
view projection matrix (the model matrix has already been assumed to have
'transformed' the 2D point). As has been mentioned a few times, inverting a
matrix is not a nice operation, either to understand or code. But! We can cheat
the inversion process again, just like we do when we create a view matrix using
the camera.

So, to form the inverted matrix, we need the aspect and fov used to create the
projection matrix of our scene, and the camera used to form the view matrix.

*/
Vector3	CollisionDetection::UnprojectScreenPosition(Vector3 position, float aspect, float fov, const Camera &c) {
	//Create our inverted matrix! Note how that to get a correct inverse matrix,
	//the order of matrices used to form it are inverted, too.
	Matrix4 invVP = GenerateInverseView(c) * GenerateInverseProjection(aspect, fov, c.GetNearPlane(), c.GetFarPlane());

	Vector2 screenSize = Window::GetWindow()->GetScreenSize();

	//Our mouse position x and y values are in 0 to screen dimensions range,
	//so we need to turn them into the -1 to 1 axis range of clip space.
	//We can do that by dividing the mouse values by the width and height of the
	//screen (giving us a range of 0.0 to 1.0), multiplying by 2 (0.0 to 2.0)
	//and then subtracting 1 (-1.0 to 1.0).
	Vector4 clipSpace = Vector4(
		(position.x / (float)screenSize.x) * 2.0f - 1.0f,
		(position.y / (float)screenSize.y) * 2.0f - 1.0f,
		(position.z) - 1.0f,
		1.0f
	);

	//Then, we multiply our clipspace coordinate by our inverted matrix
	Vector4 transformed = invVP * clipSpace;

	//our transformed w coordinate is now the 'inverse' perspective divide, so
	//we can reconstruct the final world space by dividing x,y,and z by w.
	return Vector3(transformed.x / transformed.w, transformed.y / transformed.w, transformed.z / transformed.w);
}

