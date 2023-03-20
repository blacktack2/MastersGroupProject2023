/**
 * @file   PlayerBullet.cpp
 * @brief  See PlayerBullet.h
 *
 * @author Felix Chiu
 * @date   February 2023
 */
#include "PlayerBullet.h"
#include "Boss.h"
#include "PaintRenderObject.h"
#include "InkEffectManager.h"

using namespace NCL;
using namespace CSC8503;

PlayerBullet::PlayerBullet() : Bullet() {
	inkType = NCL::InkType::PlayerDamage;
	UpdateColour();
}

PlayerBullet::PlayerBullet(PlayerBullet& other) : Bullet(other) {
	inkType = NCL::InkType::PlayerDamage;
	UpdateColour();
}

PlayerBullet::~PlayerBullet() {
}

void PlayerBullet::OnTriggerBegin(GameObject* other) {
	Bullet::OnTriggerBegin(other);
	//not work as it is not colliding with boss
	if (Boss* boss = dynamic_cast<Boss*>(other)) {
		boss->GetHealth()->Damage(0.1);
	}
}
