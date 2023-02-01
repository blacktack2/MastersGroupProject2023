#pragma once
#include "CollisionVolume.h"
#include "Vector3.h"

namespace NCL {
	using namespace Maths;
	class AABBVolume : CollisionVolume {
	public:
		AABBVolume(const Vector3& halfDims, CollisionLayer layer = CollisionLayer::Default) {
			type		= VolumeType::AABB;
			this->layer = layer;
			halfSizes	= halfDims;
		}
		~AABBVolume() {

		}

		Vector3 GetHalfDimensions() const {
			return halfSizes;
		}
	protected:
		Vector3 halfSizes;
	};
}
