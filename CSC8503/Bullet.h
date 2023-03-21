#pragma once
#include "GameObject.h"
#include "InkEffectManager.h"

namespace NCL {
	namespace CSC8503 {
		class Bullet : public GameObject {
		public:
			Bullet();
			Bullet(Bullet& other);
			~Bullet();

			virtual void Update(float dt) override;
			void OnTriggerBegin(GameObject* other) override;
			void OnCollisionBegin(GameObject* other) override;

			void UpdateColour();

			void SetLifespan(float ls) {
				lifespan = ls;
			}

			void Resize(Vector3 scale);

			void SetPaintRadius(float scale);
		protected:
			void PaintCollision(GameObject& other);

			float paintRadius = 2.0f;
			float lifespan = 0;
			Vector3 colour = Vector3(1, 0, 1);
			NCL::InkType inkType;
		};
	}
}
