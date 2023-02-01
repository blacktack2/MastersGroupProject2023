#pragma once
#include "CollisionVolume.h"

namespace NCL {
	using namespace Maths;
	class OBBVolume : CollisionVolume {
	public:
		OBBVolume(const Vector3& halfDims, CollisionLayer layer = CollisionLayer::Default) {
			type		= VolumeType::OBB;
			this->layer = layer;
			halfSizes	= halfDims;
		}
		~OBBVolume() {}

		Vector3 GetHalfDimensions() const {
			return halfSizes;
		}
	protected:
		Vector3 halfSizes;
	};
}

