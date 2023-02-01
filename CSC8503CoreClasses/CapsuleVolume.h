#pragma once
#include "CollisionVolume.h"

namespace NCL {
    class CapsuleVolume : public CollisionVolume {
    public:
        CapsuleVolume(float halfHeight, float radius, CollisionLayer layer = CollisionLayer::Default) {
            this->halfHeight = halfHeight;
            this->radius     = radius;
            this->type       = VolumeType::Capsule;
			this->layer      = layer;
        };
        ~CapsuleVolume() {

        }

        float GetRadius() const {
            return radius;
        }

        float GetHalfHeight() const {
            return halfHeight;
        }
    protected:
        float radius;
        float halfHeight;
    };
}

