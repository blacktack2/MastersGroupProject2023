/**
 * @file   CollisionVolume.h
 * @brief  A file to define collision volume and their methods and arguments.
 * 
 * @author Shashwat Kashyap
 * @author Stuart Lewis
 * @date   March 2023
 */
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
		virtual ~CollisionVolume() {}

		CollisionLayer GetCollisionLayer() {
			return layer;
		}

		static CollisionVolume* Clone(CollisionVolume& other);

		VolumeType type;
		CollisionLayer layer;
	};
}