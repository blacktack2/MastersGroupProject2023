#include "Bullet.h"

#include "CapsuleVolume.h"
#include "PaintRenderObject.h"
#include "GameGridManager.h"

#include "Boss.h"

#include "Debug.h"

using namespace NCL;
using namespace CSC8503;

Bullet::Bullet() : GameObject() {
}

Bullet::Bullet(Bullet& other, paintHell::InkType inkType) : GameObject(other) {
	lifespan = other.lifespan;
	this->inkType = inkType;
}

Bullet::~Bullet() {
}

void Bullet::Update(float dt) {
	lifespan -= dt;
	if (lifespan <= 0) {
		Delete();
		return;
	}

	//Debug::DrawLine(transform.GetGlobalPosition(), transform.GetGlobalPosition() + Vector3(0, 0.01f , 0), Vector4(0, 1, 1, 1));
}

void Bullet::OnCollisionBegin(GameObject* other) {
	if (other->GetBoundingVolume()->layer == CollisionLayer::PaintAble)
	{
		PaintRenderObject* renderObj = (PaintRenderObject*)other->GetRenderObject();
		renderObj->AddPaintCollision(PaintCollision(transform.GetGlobalPosition(), 3.0f));
	}
	//not work as it is not colliding with boss
	if (Boss* boss = dynamic_cast<Boss*>(other)) {
		
		boss->GetHealth()->Damage(10);
		std::cout << "hit boss "<< boss->GetHealth() << std::endl;
	}
	GameGridManager::instance().PaintPosition(GetTransform().GetGlobalPosition(), inkType);
	Delete();
}
