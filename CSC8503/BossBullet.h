#pragma once
#include "Bullet.h"
#include "GameObject.h"

namespace NCL {
	namespace CSC8503 {
		class BossBullet : public Bullet {
		public:
			BossBullet();
			BossBullet(BossBullet& other, paintHell::InkType inkType);
			~BossBullet();

			void Update(float dt);
			void OnCollisionBegin(GameObject* other);

			void SetLifeSpan(float life)
			{
				lifeSpan = life;
			}
			void SetDamage(float damage)
			{
				bulletDamage = damage;
			}
			float GetDamage()
			{
				return bulletDamage;
			}
		protected:
			Vector3 velocity{ 0,0,0 };
			float lifeSpan = 0.0f;
			float bulletDamage = 5.0f;
		};
	}
}
