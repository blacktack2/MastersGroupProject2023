/**
 * @file   Player Bullet.cpp
 * @brief  A Class for defining Player projectile behaviour
 *
 * @author Felix Chiu
 * @date   February 2023
 */
#pragma once
#include "Bullet.h"
#ifdef x64
#include "GameObject.h"
#endif // x64

#ifdef _ORBIS
#include "../Common/GameObject.h"
#endif // _ORBIS




namespace NCL {
	namespace CSC8503 {
		class PlayerBullet : public Bullet {
		public:
			PlayerBullet();
			PlayerBullet(PlayerBullet& other);
			~PlayerBullet();

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
