#include "Bullet.h"
#include "PaintRenderObject.h"
#include "InkEffectManager.h"
#include "Debug.h"
#include "GameGridManager.h"
#include "SphereVolume.h"

using namespace NCL;
using namespace CSC8503;

Bullet::Bullet() : GameObject() {
}

Bullet::Bullet(Bullet& other) : GameObject(other) {
	lifespan = other.lifespan;
	inkType = NCL::InkType::NoInk;
}

Bullet::~Bullet() {
}

void Bullet::Update(float dt) {
	lifespan -= dt;
	if (lifespan <= 0) {
		isActive = false;
		return;
	}

	//Debug::DrawLine(transform.GetGlobalPosition(), transform.GetGlobalPosition() + Vector3(0, 0.01f , 0), Vector4(0, 1, 1, 1));
}

void Bullet::OnCollisionBegin(GameObject* other) {
	if (other->GetBoundingVolume()->layer == CollisionLayer::PaintAble)
	{
		PaintRenderObject* renderObj = (PaintRenderObject*)other->GetRenderObject();
		renderObj->AddPaintCollision(PaintCollision(transform.GetGlobalPosition(), paintRadius, colour));
		GameGridManager::instance().PaintPosition(GetTransform().GetGlobalPosition(), paintRadius, inkType);
	}
	boundingVolume = (CollisionVolume*) new SphereVolume(paintRadius, boundingVolume->layer);
	lifespan = -1;
}

void Bullet::OnTriggerBegin(GameObject* other) {
	if (other->GetBoundingVolume()->layer == CollisionLayer::PaintAble)
	{
		PaintRenderObject* renderObj = (PaintRenderObject*)other->GetRenderObject();
		renderObj->AddPaintCollision(PaintCollision(transform.GetGlobalPosition(), paintRadius, colour));
		GameGridManager::instance().PaintPosition(GetTransform().GetGlobalPosition(), paintRadius, inkType);
	}
	boundingVolume = (CollisionVolume*) new SphereVolume(paintRadius, boundingVolume->layer);
	lifespan = -1;
}

void Bullet::UpdateColour() {
	colour = InkEffectManager::instance().GetColour(inkType);
	if (this->GetRenderObject()) {
		this->GetRenderObject()->SetColour(Vector4(colour, 1));
	}
	
}

void Bullet::Resize(Vector3 scale) {
	CollisionLayer layer = boundingVolume->GetCollisionLayer();
	delete boundingVolume;
	boundingVolume = (CollisionVolume*) new SphereVolume(scale.x, layer);
	transform.SetScale(scale);
}
