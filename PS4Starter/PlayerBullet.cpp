/**
 * @file   PlayerBullet.cpp
 * @brief  See PlayerBullet.h
 *
 * @author Felix Chiu
 * @date   February 2023
 */
#include "PlayerBullet.h"
#ifdef x64
#include "Boss.h"
#include "PaintRenderObject.h"
#include "InkEffectManager.h"

#endif // x64

#ifdef _ORBIS
#include"Boss.h"
#endif // _ORBIS

using namespace NCL;
using namespace CSC8503;

PlayerBullet::PlayerBullet() : Bullet() {
	inkType = paintHell::InkType::PlayerDamage;
	UpdateColour();
}

PlayerBullet::PlayerBullet(PlayerBullet& other) : Bullet(other) {
	inkType = paintHell::InkType::PlayerDamage;
	UpdateColour();
}

PlayerBullet::~PlayerBullet() {
}

void PlayerBullet::OnTriggerBegin(GameObject* other) {
	Bullet::OnTriggerBegin(other);
	//not work as it is not colliding with boss
	if (Boss* boss = dynamic_cast<Boss*>(other)) {
		boss->GetHealth()->Damage(10);

	}
}
