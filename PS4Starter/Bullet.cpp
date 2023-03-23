#include "Bullet.h"
#ifdef x64
#include "PaintRenderObject.h"
#include "InkEffectManager.h"
#include "Debug.h"
#include "GameGridManager.h"
#include "SphereVolume.h"
#endif // x64


#ifdef _ORBIS
#include"InkEffectManager.h"
#include "../Common/Debug.h"
#include "GameGridManager.h"
#include "../Common/SphereVolume.h"
#include "../Common/RenderObject.h"
#include "../Common/PhysicsObject.h"
#endif // _ORBIS


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
	if (lifespan < 0) {
		isActive = false;
		GetPhysicsObject()->ClearForces();
		GetPhysicsObject()->SetLinearVelocity(Vector3(0.0f));
		GetPhysicsObject()->SetAngularVelocity(Vector3(0.0f));
		physicsObject->SetInverseMass(1.0f);
		return;
	}
	lifespan -= dt;

	//Debug::DrawLine(transform.GetGlobalPosition(), transform.GetGlobalPosition() + Vector3(0, 0.01f , 0), Vector4(0, 1, 1, 1));
}

void Bullet::OnCollisionBegin(GameObject* other) {
	if (other->GetBoundingVolume()->layer == CollisionLayer::PaintAble)
	{
		//PaintRenderObject* renderObj = (PaintRenderObject*)other->GetRenderObject();
		//renderObj->AddPaintCollision(PaintCollision(transform.GetGlobalPosition(), paintRadius, colour)); //->to be implemented
		PaintCollision(*other);
		GameGridManager::instance().PaintPosition(GetTransform().GetGlobalPosition(), paintRadius, inkType);
	}
	if (lifespan > 0)
	{
		boundingVolume = (CollisionVolume*) new SphereVolume(paintRadius, boundingVolume->layer);
		lifespan = -1;
	}
}

void Bullet::OnTriggerBegin(GameObject* other) {
	if (!other) {
		return;
	}
	PaintCollision(*other);
	SetBoundingVolume((CollisionVolume*) new SphereVolume(paintRadius, boundingVolume->layer));
	physicsObject->SetInverseMass(0);
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
	delete boundingVolume;
	boundingVolume = (CollisionVolume*) new SphereVolume(scale.x, layer);
	transform.SetScale(scale);
}

void Bullet::PaintCollision(GameObject& other) {
	if (other.GetBoundingVolume()->layer != CollisionLayer::PaintAble) {
		return;
	}
	RenderObject* renderObj = other.GetRenderObject();
	/*if (!(renderObj && renderObj->GetPaintTexture())) {
		return;
	}*/
	//renderObj->AddPaintCollision(transform.GetGlobalPosition(), paintRadius, colour);
	//GameGridManager::instance().PaintPosition(GetTransform().GetGlobalPosition(), paintRadius, inkType);
}
