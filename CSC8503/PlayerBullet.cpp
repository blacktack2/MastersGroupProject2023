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
#include "GameGridManager.h"

using namespace NCL;
using namespace CSC8503;

PlayerBullet::PlayerBullet() : Bullet() {
}

PlayerBullet::PlayerBullet(PlayerBullet& other, paintHell::InkType inkType) : Bullet(other, inkType) {
}

PlayerBullet::~PlayerBullet() {
}

void PlayerBullet::Update(float dt) {
	Bullet::Update(dt);
}

void PlayerBullet::OnTriggerBegin(GameObject* other) {
	Bullet::OnTriggerBegin(other);
	//not work as it is not colliding with boss
	if (Boss* boss = dynamic_cast<Boss*>(other)) {

		boss->GetHealth()->Damage(10);

	}
	GameGridManager::instance().PaintPosition(GetTransform().GetGlobalPosition(), inkType);
	lifeSpan = -1;

}
