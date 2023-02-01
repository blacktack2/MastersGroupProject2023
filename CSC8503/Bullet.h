#pragma once
#include "GameObject.h"

namespace NCL {
	namespace CSC8503 {
		class Bullet : public GameObject {
		public:
			Bullet(GameWorld& gameWorld);
			Bullet(GameWorld& gameWorld, Bullet& other);
			~Bullet();

			virtual void Update(float dt) override;

			void SetLifespan(float ls) {
				lifespan = ls;
			}
		protected:
			float lifespan = 0;
		};
	}
}
