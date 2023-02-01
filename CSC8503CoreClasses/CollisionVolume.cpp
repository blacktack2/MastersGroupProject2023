#include "CollisionVolume.h"

#include "AABBVolume.h"
#include "OBBVolume.h"
#include "SphereVolume.h"
#include "CapsuleVolume.h"

using namespace NCL;

CollisionVolume* CollisionVolume::Clone(CollisionVolume& other) {
	switch (other.type) {
		case VolumeType::AABB:
			return (CollisionVolume*) new AABBVolume((AABBVolume&)other);
		case VolumeType::OBB:
			return (CollisionVolume*) new OBBVolume((OBBVolume&)other);
		case VolumeType::Sphere:
			return (CollisionVolume*) new SphereVolume((SphereVolume&)other);
		case VolumeType::Capsule:
			return (CollisionVolume*) new CapsuleVolume((CapsuleVolume&)other);
	}
	return nullptr;
}
