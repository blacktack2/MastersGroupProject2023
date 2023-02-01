#pragma once
#include "Collision.h"

namespace NCL {
	enum class VolumeType {
		AABB	= 1,
		OBB		= 2,
		Sphere	= 4, 
		Mesh	= 8,
		Capsule = 16,
		Compound= 32,
		Invalid = 256
	};

	class CollisionVolume {
	public:
		CollisionVolume() {
			type = VolumeType::Invalid;
			layer = CollisionLayer::Default;
		}
		~CollisionVolume() {}

		static CollisionVolume* Clone(CollisionVolume& other);

		VolumeType type;
		CollisionLayer layer;
	};
}