#include "Bullet.h"
#include "PaintRenderObject.h"
#include "InkEffectManager.h"
#include "Debug.h"
#include "GameGridManager.h"

using namespace NCL;
using namespace CSC8503;

Bullet::Bullet() : GameObject() {
}

Bullet::Bullet(Bullet& other) : GameObject(other) {
	lifespan = other.lifespan;
	inkType = paintHell::InkType::NoInk;
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
		renderObj->AddPaintCollision(PaintCollision(transform.GetGlobalPosition(), 1.5f, colour));
	}
	GameGridManager::instance().PaintPosition(GetTransform().GetGlobalPosition(), inkType);
	lifespan = -1;
}

void Bullet::OnTriggerBegin(GameObject* other) {
	if (other->GetBoundingVolume()->layer == CollisionLayer::PaintAble)
	{
		PaintRenderObject* renderObj = (PaintRenderObject*)other->GetRenderObject();
		renderObj->AddPaintCollision(PaintCollision(transform.GetGlobalPosition(), 1.5f, colour));
	}
	GameGridManager::instance().PaintPosition(GetTransform().GetGlobalPosition(), inkType);
	lifespan = -1;
}

void Bullet::UpdateColour() {
	colour = InkEffectManager::instance().GetColour(inkType);
	if (this->GetRenderObject()) {
		this->GetRenderObject()->SetColour(Vector4(colour, 1));
	}
	
}
