/**
 * @file   Player Bullet.cpp
 * @brief  A Class for defining Player projectile behaviour
 *
 * @author Felix Chiu
 * @date   February 2023
 */
#pragma once
#include "Bullet.h"
#include "GameObject.h"

namespace NCL {
	namespace CSC8503 {
		class PlayerBullet : public Bullet {
		public:
			PlayerBullet();
			PlayerBullet(PlayerBullet& other, paintHell::InkType inkType);
			~PlayerBullet();

			void Update(float dt);
			void OnTriggerBegin(GameObject* other) override;

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
