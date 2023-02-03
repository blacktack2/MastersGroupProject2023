#pragma once
#include "PlayerObject.h"

#include "AssetLibrary.h"
#include "Bullet.h"
#include "Bonus.h"
#include "Constraint.h"
#include "GameWorld.h"
#include "NPCObject.h"
#include "PhysicsObject.h"
#include "Window.h"

#include "RenderObject.h"

#include "InputKeyMap.h"

#include <iostream>

using namespace NCL;
using namespace CSC8503;

PlayerObject::PlayerObject(int id, int& scoreCounter) : GameObject(GameWorld::instance()),
id(id), scoreCounter(scoreCounter){
	OnCollisionBeginCallback = [&](GameObject* other) {
		CollisionWith(other);
	};

}

PlayerObject::~PlayerObject() {

}

void PlayerObject::Update(float dt) {
	jumpTimer -= dt;
	projectileFireRateTimer -= dt;
	RotateToCamera();
	CheckGround();
	Move();
	MoveCamera();
}

void PlayerObject::MoveTo(Vector3 position) {
	Vector3 diff = position - this->GetTransform().GetGlobalPosition();
	
	if (diff.Length() > 0.1f) {
		Vector3 dir = (position - this->GetTransform().GetGlobalPosition()).Normalised();
		this->GetPhysicsObject()->ApplyLinearImpulse(dir * moveSpeed);
	}
		
}

void PlayerObject::Move() {
	Vector3 dir = Vector3(0,0,0);
	GetInput(dir);
	this->GetPhysicsObject()->ApplyLinearImpulse(dir * moveSpeed);
	if (lastDir != Vector3(0, 0, 0)) {
		//Vector3 stopDir = dir - lastDir;
		if (paintHell::InputKeyMap::instance().GetButtonState() != lastKey) {
			this->GetPhysicsObject()->ApplyLinearImpulse(-lastDir * moveSpeed);
		}
		
	}

	lastDir = dir;
}

void PlayerObject::MoveCamera() {
	if (hasCamera) {
		//gameWorld.GetMainCamera()->SetPosition(transform.GetGlobalPosition() + cameraOffset);
	}
}

void PlayerObject::GetInput(Vector3& dir) {
	paintHell::InputKeyMap& keyMap = paintHell::InputKeyMap::instance();
	lastKey = keyMap.GetButtonState();
	keyMap.Update();
	Vector3 fwdAxis = this->GetTransform().GetGlobalOrientation() * Vector3(0, 0, -1);

	Vector3 leftAxis = this->GetTransform().GetGlobalOrientation() * Vector3(-1, 0, 0);

	Vector3 upAxis = this->GetTransform().GetGlobalOrientation() * Vector3(0, 1, 0);

	isFreeLook = false;

	if (keyMap.GetButton(InputType::Foward)) 
	{
		dir += fwdAxis;
	}
	if (keyMap.GetButton (InputType::Backward) )
	{
		dir -= fwdAxis;
	}
	if (keyMap.GetButton(InputType::Left)) 
	{
		dir += leftAxis;
	}
	if (keyMap.GetButton(InputType::Right)) 
	{
		dir -= leftAxis;
	}
	if (keyMap.GetButton(InputType::Jump) && onGround && jumpTimer <= 0.0f ) 
	{
		jumpTimer = jumpCooldown;
		this->GetPhysicsObject()->ApplyLinearImpulse(upAxis * jumpSpeed);
	}
	if (keyMap.GetButton(InputType::Action1)) 
	{
		Shoot();
	}
	if (keyMap.GetButton(InputType::FreeLook))
	{
		isFreeLook = true;
	}
	dir.Normalise();
}

void PlayerObject::CheckGround() {
	Ray r = Ray(this->GetTransform().GetGlobalPosition(), Vector3(0,-1,0));
	RayCollision closestCollision;
	GameObject* objClosest;
	onGround = false;
	if (gameWorld.Raycast(r, closestCollision, true, this)) 
	{
		objClosest = (GameObject*)closestCollision.node;
		float groundDist = (closestCollision.collidedAt - this->GetTransform().GetGlobalPosition()).Length();
		//std::cout << "ground dist " << groundDist << std::endl;
		if (groundDist < 1.02f)
		{
			
			onGround = true;
		}	
	}
}

void PlayerObject::RotateYaw(float yaw) {
	this->GetTransform().SetOrientation(Quaternion::Lerp(this->GetTransform().GetGlobalOrientation(), Quaternion::AxisAngleToQuaterion(Vector3(0, 1, 0), yaw), camTurnSpeed));
}

void PlayerObject::RotateToCamera() {
	if (hasCamera && !isFreeLook) {
		RotateYaw(gameWorld.GetMainCamera()->GetYaw());
	}
	
}

//legacy 
void PlayerObject::AddPoints(int points) {
	if (lastGoosed > gooseDelay) {
		lastGoosed = 0.0f;
		scoreCounter += points;
	}
}

void PlayerObject::CollisionWith(GameObject* other) {
	/*
	if (other->GetPhysicsObject() != nullptr && !collidedWith.contains(other->GetWorldID()) && !other->GetPhysicsObject()->IsStatic()) {
		collidedWith.insert(other->GetWorldID());
		if (dynamic_cast<NPCObject*>(other)) {
			scoreCounter += 500;
		} else {
			scoreCounter += 100;
		}
	}
	*/
	//gameWorld.RemoveGameObject(this);
}


void PlayerObject::Shoot() {
	if (projectileFireRateTimer>0)
		return;
	projectileFireRateTimer = projectileFireRate;
	Bullet* ink = new Bullet(gameWorld, *(Bullet*)AssetLibrary::GetPrefab("bullet"));
	ink->SetLifespan(projectileLifespan);
	ink->GetTransform().SetPosition(transform.GetGlobalOrientation() * projectileSpawnPoint + transform.GetGlobalPosition());
	ink->GetPhysicsObject()->ApplyLinearImpulse(transform.GetGlobalOrientation() * Vector3(0, 0, -1) * projectileForce);
	gameWorld.AddGameObject(ink);
	ink->OnCollisionBeginCallback = [&](GameObject* other) {
		CollisionWith(other);
	};
}