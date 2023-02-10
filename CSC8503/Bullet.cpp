#include "Bullet.h"

#include "CapsuleVolume.h"
#include "PaintRenderObject.h"

#include "Debug.h"

using namespace NCL;
using namespace CSC8503;

Bullet::Bullet() : GameObject() {
}

Bullet::Bullet(Bullet& other) : GameObject(other) {
	lifespan = other.lifespan;
}

Bullet::~Bullet() {
}

void Bullet::Update(float dt) {
	lifespan -= dt;
	if (lifespan <= 0) {
		Delete();
		return;
	}

	Debug::DrawLine(transform.GetGlobalPosition(), transform.GetGlobalPosition() + Vector3(0, 0.01f , 0), Vector4(0, 1, 1, 1));
}

void Bullet::OnCollisionBegin(GameObject* other) {
	if (other->GetBoundingVolume()->layer == CollisionLayer::PaintAble)
	{
		PaintRenderObject* renderObj = (PaintRenderObject*)other->GetRenderObject();
		renderObj->AddPaintCollision(PaintCollision(transform.GetGlobalPosition(), 5.0f));
	}
}
