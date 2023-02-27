/**
 * @file   BossBullet.cpp
 * @brief  A Class for defining boss projectile behaviour
 *
 * @author Xiaoyang Liu
 * @author Felix Chiu
 * @date   February 2023
 */
#pragma once
#include "Bullet.h"
#include "GameObject.h"

namespace NCL {
	namespace CSC8503 {
		class BossBullet : public Bullet {
		public:
			BossBullet();
			BossBullet(BossBullet& other);
			~BossBullet();

			void Update(float dt) override;
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
			float bulletDamage = 5.0f;
		};
	}
}
