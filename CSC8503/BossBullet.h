#pragma once
#include "Bullet.h"
#include "GameObject.h"

namespace NCL {
	namespace CSC8503 {
		class BossBullet : public Bullet {
		public:
			BossBullet(Vector3 vel);
			~BossBullet();

			virtual void Update(float dt) override;

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

			void SetVelocity(Vector3 vel)
			{
				velocity = vel;
			}

		protected:
			Vector3 velocity{ 0,0,0 };
			float lifeSpan = 0.0f;
			float bulletDamage = 5.0f;
		};
	}
}
