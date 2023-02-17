#pragma once
#include "GameObject.h"

namespace NCL {
	namespace CSC8503 {
		class Bullet : public GameObject {
		public:
			Bullet();
			Bullet(Bullet& other);
			~Bullet();

			virtual void Update(float dt) override;

			void OnCollisionBegin(GameObject* other) override;

			void OnTriggerBegin(GameObject* otherObject) override;

			void SetLifespan(float ls) {
				lifespan = ls;
			}
		protected:
			float lifespan = 0;
			Vector3 colour = Vector3(1, 0, 1);
		};
	}
}
