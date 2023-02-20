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
	inkType = paintHell::InkType::BossDamage;
	UpdateColour();
}

BossBullet::BossBullet(BossBullet& other) : Bullet(other) {
	inkType = paintHell::InkType::BossDamage;
	UpdateColour();
}

BossBullet::~BossBullet() {
}

void BossBullet::Update(float dt) {
	/*if (this->GetTransform().GetGlobalPosition().y < 1.0f)
	{
		GetTransform().SetPosition({ GetTransform().GetGlobalPosition().x, 1.0f, GetTransform().GetGlobalPosition().z });
	}*/
	Bullet::Update(dt);
}

void BossBullet::OnTriggerBegin(GameObject* other) {
	//delete if colliding with boss
	if (!dynamic_cast<Boss*>(other)) {
		Bullet::OnTriggerBegin(other);
	}
	
	if (PlayerObject* player = dynamic_cast<PlayerObject*>(other)) {
		player->GetHealth()->Damage(10);
	}
	if (Obstacle* obj = dynamic_cast<Obstacle*>(other)) {
		obj->Damage(10);
	}
}

