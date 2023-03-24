/**
 * @file   CapsuleVolume.h
 * @brief  A file to store capsule volume methods and arguments.
 * 
 * @author Shashwat Kashyap
 * @author Stuart Lewis
 * @date   March 2023
 */
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

