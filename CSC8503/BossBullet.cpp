#include "BossBullet.h"
#include "Boss.h"
#include "PaintRenderObject.h"
#include "InkEffectManager.h"
#include "GameGridManager.h"

using namespace NCL;
using namespace CSC8503;

BossBullet::BossBullet() : Bullet() {
}

BossBullet::BossBullet(BossBullet& other, paintHell::InkType inkType) : Bullet(other) {
    lifespan = other.lifespan;
    this->inkType = inkType;
}

BossBullet::~BossBullet() {
}

void BossBullet::Update(float dt) {
	Debug::DrawLine(this->GetTransform().GetGlobalPosition(), this->GetTransform().GetGlobalPosition() + Vector3(0, 0.5f, 0), Debug::YELLOW, 1.0f);
    lifespan -= dt;
    if (lifespan <= 0) {
        Delete();
        return;
    }
}

void BossBullet::OnCollisionBegin(GameObject* other) {
	if (other->GetBoundingVolume()->layer == CollisionLayer::PaintAble)
	{
		PaintRenderObject* renderObj = (PaintRenderObject*)other->GetRenderObject();
		renderObj->AddPaintCollision(PaintCollision(transform.GetGlobalPosition(), 3.0f));
	}
	//not work as it is not colliding with boss
	if (Boss* boss = dynamic_cast<Boss*>(other)) {

		boss->GetHealth()->Damage(10);
	}
	GameGridManager::instance().PaintPosition(GetTransform().GetGlobalPosition(), inkType);
	Delete();
}
