/**
 * @file   BossBullet.cpp
 * @brief  See BossBullet.h
 *
 * @author Xiaoyang Liu
 * @author Felix Chiu
 * @date   February 2023
 */
#include "BossBullet.h"
#include "Boss.h"
#include "PaintRenderObject.h"
#include "InkEffectManager.h"
#include "GameGridManager.h"

using namespace NCL;
using namespace CSC8503;

BossBullet::BossBullet() : Bullet() {
}

BossBullet::BossBullet(BossBullet& other, paintHell::InkType inkType) : Bullet(other, inkType) {
}

BossBullet::~BossBullet() {
}

void BossBullet::Update(float dt) {
	Bullet::Update(dt);
}
void BossBullet::OnTriggerBegin(GameObject* other) {
	Bullet::OnTriggerBegin(other);
	//delete if colliding with boss
	if (!dynamic_cast<Boss*>(other)) {
		GameGridManager::instance().PaintPosition(GetTransform().GetGlobalPosition(), inkType);
		lifeSpan = -1;
	}

}

