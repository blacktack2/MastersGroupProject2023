/**
 * @file   BossBullet.cpp
 * @brief  A Class for defining boss projectile behaviour
 *
 * @author Xiaoyang Liu
 * @author Felix Chiu
 * @date   February 2023
 */
#pragma once
#ifdef x64
#include "Bullet.h"
#include "GameObject.h"
#endif // x64

#ifdef _ORBIS
#include "Bullet.h"
#include "../Common/GameObject.h"
#endif // _ORBIS


namespace NCL {
	namespace CSC8503 {
		class BossBullet : public Bullet {
		public:
			BossBullet();
			BossBullet(BossBullet& other);
			~BossBullet();

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
