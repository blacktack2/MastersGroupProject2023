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

PlayerObject::PlayerObject(GameWorld& gameWorld, int id, int& scoreCounter) : GameObject(gameWorld),
id(id), scoreCounter(scoreCounter){
	OnCollisionBeginCallback = [&](GameObject* other) {
		CollisionWith(other);
	};

}

PlayerObject::~PlayerObject() {

}

void PlayerObject::Update(float dt) {
	jumpTimer -= dt;
	CheckGround();
	Move();
}

void PlayerObject::Move() {
	Vector3 dir = Vector3(0,0,0);
	GetInput(dir);
	this->GetPhysicsObject()->ApplyLinearImpulse(dir * moveSpeed);
}

void PlayerObject::GetInput(Vector3& dir) {
	paintHell::InputKeyMap& keyMap = paintHell::InputKeyMap::instance();
	keyMap.Update();
	if (keyMap.GetButton(InputType::Foward)) 
	{
		dir += Vector3(0, 0, -1);
	}
	if (keyMap.GetButton (InputType::Backward) )
	{
		dir += Vector3(0, 0, 1);
	}
	if (keyMap.GetButton(InputType::Left)) 
	{
		dir += Vector3(-1, 0, 0);
	}
	if (keyMap.GetButton(InputType::Right)) 
	{
		dir += Vector3(1, 0, 0);
	}
	if (keyMap.GetButton(InputType::Jump) && onGround && jumpTimer <= 0.0f ) 
	{
		jumpTimer = jumpCooldown;
		std::cout << "JUMP" << std::endl;
		this->GetPhysicsObject()->ApplyLinearImpulse(Vector3(0, 1, 0) * jumpSpeed);
	}
	if (keyMap.GetButton(InputType::Action1)) 
	{
		Shoot();
	}
	dir.Normalise();
}

void PlayerObject::CheckGround() {
	Ray r = Ray(this->GetTransform().GetPosition(), Vector3(0,-1,0));
	RayCollision closestCollision;
	GameObject* objClosest;
	onGround = false;
	if (gameWorld.Raycast(r, closestCollision, true, this)) 
	{
		objClosest = (GameObject*)closestCollision.node;
		float groundDist = (closestCollision.collidedAt - this->GetTransform().GetPosition()).Length();
		//std::cout << "ground dist " << groundDist << std::endl;
		if (groundDist < 1.02f)
		{
			
			onGround = true;
		}	
	}
}

void PlayerObject::AddPoints(int points) {
	if (lastGoosed > gooseDelay) {
		lastGoosed = 0.0f;
		scoreCounter += points;
	}
}

void PlayerObject::CollisionWith(GameObject* other) {
	if (other->GetPhysicsObject() != nullptr && !collidedWith.contains(other->GetWorldID()) && !other->GetPhysicsObject()->IsStatic()) {
		collidedWith.insert(other->GetWorldID());
		if (dynamic_cast<NPCObject*>(other)) {
			scoreCounter += 500;
		} else {
			scoreCounter += 100;
		}
	}
}
/*
void PlayerObject::HandleGroundInput(float dt) {
	const float moveForce = 40;
	const float rotateTorque = 4;
	const float jumpForce = 1000;
	const Vector3 jumpTorque = Vector3(1, 0, 1) * 500;
	float forwardThrust = 0;
	float upwardThrust = 0;
	Vector3 rotation = Vector3(0);
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A)) {
		rotation.y += rotateTorque;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D)) {
		rotation.y -= rotateTorque;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W)) {
		forwardThrust -= moveForce;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S)) {
		forwardThrust += moveForce;
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE)) {
		upwardThrust += jumpForce;
		rotation += Vector3((rand() * (1.0f / (float)RAND_MAX)), 0, (rand() * (1.0f / (float)RAND_MAX))) * jumpTorque;
	}
	physicsObject->AddForce(transform.GetOrientation() * Vector3(0, 0, forwardThrust) + Vector3(0, upwardThrust, 0));
	physicsObject->AddTorque(rotation);
}

void PlayerObject::HandleGoatActions(float dt) {
	laserDelay -= dt;
	if (laserDelay < 0 && Window::GetKeyboard()->KeyDown(KeyboardKeys::R)) {
		laserDelay = laserFireRate;
		FireLasers();
	}

	if (grappledObject) {
		Vector3 toungePosition = transform.GetPosition() + transform.GetOrientation() * toungePos;
		Vector3 contactPosition = grappledObject->GetTransform().GetPosition() + toungeContactPoint;
		Vector3 delta = contactPosition - toungePosition;
		Vector3 deltaNorm = delta.Normalised();
		float distance = delta.Length();
		static const float as = std::sin(3.14159 * 0.25), ac = std::cos(3.14159 * 0.25);
		tounge->GetTransform().SetPosition(toungePosition + delta * 0.5f)
			                  .SetOrientation(Quaternion(Vector3::Cross(deltaNorm, Vector3(0, 1, 0)) * as, ac))
			                  .SetScale(Vector3(0.1f, distance, 0.1f));
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::E)) { // Release grappled object
			gameWorld.RemoveConstraint(grappleConstraint, true);
			grappledObject = nullptr;
			tounge->SetActive(false);
		}
	} else {
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::E)) { // Grapple a new object
			RayCollision collision;
			Ray r = Ray(transform.GetPosition() + toungePos, transform.GetOrientation() * Vector3(0, 0, -1), CollisionLayer::Player);
			if (gameWorld.Raycast(r, collision, true, this) && collision.rayDistance <= toungeMaxDistance) {
				grappledObject = (GameObject*)collision.node;
				toungeContactPoint = collision.collidedAt - grappledObject->GetTransform().GetPosition();
				grappleConstraint = new PositionConstraint(this, grappledObject, 0, toungeMaxDistance, Vector3(0), toungeContactPoint);
				gameWorld.AddConstraint(grappleConstraint);
				tounge->SetActive(true);

				CollisionWith(grappledObject);
			}
		}
	}
}
*/

void PlayerObject::Shoot() {
	Bullet* laserA = new Bullet(gameWorld, *(Bullet*)AssetLibrary::GetPrefab("bullet"));
	laserA->SetLifespan(laserLifespan);
	laserA->GetTransform().SetPosition(transform.GetOrientation() * eyePosL + transform.GetPosition());
	laserA->GetPhysicsObject()->AddForce(transform.GetOrientation() * laserForce);
	gameWorld.AddGameObject(laserA);
	laserA->OnCollisionBeginCallback = [&](GameObject* other) {
		CollisionWith(other);
	};

	Bullet* laserB = new Bullet(gameWorld, *(Bullet*)AssetLibrary::GetPrefab("bullet"));
	laserB->SetLifespan(laserLifespan);
	laserB->GetTransform().SetPosition(transform.GetOrientation() * eyePosR + transform.GetPosition());
	laserB->GetPhysicsObject()->AddForce(transform.GetOrientation() * laserForce);
	gameWorld.AddGameObject(laserB);
	laserB->OnCollisionBeginCallback = [&](GameObject* other) {
		CollisionWith(other);
	};
}