#pragma once
#include "CollisionVolume.h"

namespace NCL {
	class SphereVolume : CollisionVolume {
	public:
		SphereVolume(float sphereRadius = 1.0f, CollisionLayer layer = CollisionLayer::Default) {
			type	    = VolumeType::Sphere;
			this->layer = layer;
			radius	    = sphereRadius;
		}
		~SphereVolume() {}

		float GetRadius() const {
			return radius;
		}
	protected:
		float	radius;
	};
}

