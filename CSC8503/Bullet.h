#pragma once
#include "GameObject.h"
#include "InkEffectManager.h"

namespace NCL {
	namespace CSC8503 {
		class Bullet : public GameObject {
		public:
			Bullet();
			Bullet(Bullet& other, paintHell::InkType inkType);
			~Bullet();

			virtual void Update(float dt) override;

			void OnCollisionBegin(GameObject* other) override;

			void SetLifespan(float ls) {
				lifespan = ls;
			}
		protected:
			float lifespan = 0;
			paintHell::InkType inkType;
		};
	}
}
