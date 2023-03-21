#include "Bullet.h"

#include "Debug.h"
#include "GameGridManager.h"
#include "InkEffectManager.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "SphereVolume.h"

using namespace NCL;
using namespace NCL::CSC8503;

Bullet::Bullet() : GameObject() {
}

Bullet::Bullet(Bullet& other) : GameObject(other) {
	lifespan = other.lifespan;
	inkType = NCL::InkType::NoInk;
	paintRadius = other.paintRadius;
}

Bullet::~Bullet() {
}

void Bullet::Update(float dt) {
	if (lifespan < 0) {
		isActive = false;
		GetPhysicsObject()->ClearForces();
		GetPhysicsObject()->SetLinearVelocity(Vector3(0.0f));
		GetPhysicsObject()->SetAngularVelocity(Vector3(0.0f));
		return;
	}
	lifespan -= dt;

	//Debug::DrawLine(transform.GetGlobalPosition(), transform.GetGlobalPosition() + Vector3(0, 0.01f , 0), Vector4(0, 1, 1, 1));
}

void Bullet::OnCollisionBegin(GameObject* other) {
	if (!other) {
		return;
	}
	PaintCollision(*other);
	SetBoundingVolume((CollisionVolume*) new SphereVolume(paintRadius, boundingVolume->layer));
	lifespan = 0;
}

void Bullet::OnTriggerBegin(GameObject* other) {
	if (!other) {
		return;
	}
	PaintCollision(*other);
	SetBoundingVolume((CollisionVolume*) new SphereVolume(paintRadius, boundingVolume->layer));
	lifespan = 0;
}

void Bullet::UpdateColour() {
	colour = InkEffectManager::instance().GetColour(inkType);
	if (this->GetRenderObject()) {
		this->GetRenderObject()->SetColour(Vector4(colour, 1));
	}
	
}

void Bullet::Resize(Vector3 scale) {
	CollisionLayer layer = boundingVolume->GetCollisionLayer();
	SetBoundingVolume( (CollisionVolume*) new SphereVolume(scale.x, layer));
	transform.SetScale(scale);
}

void Bullet::SetPaintRadius(float scale) {
	paintRadius = scale;
}

void Bullet::PaintCollision(GameObject& other) {
	if (other.GetBoundingVolume()->layer != CollisionLayer::PaintAble) {
		return;
	}
	RenderObject* renderObj = other.GetRenderObject();
	if (!(renderObj && renderObj->GetPaintTex())) {
		return;
	}
	renderObj->AddPaintCollision(transform.GetGlobalPosition(), paintRadius, colour);
	GameGridManager::instance().PaintPosition(GetTransform().GetGlobalPosition(), paintRadius, inkType);
}
