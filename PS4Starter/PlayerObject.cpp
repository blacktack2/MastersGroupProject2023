#pragma once
#include "PlayerObject.h"
#ifdef x64
#include "AssetLibrary.h"
#include "PrefabLibrary.h"

#include "PlayerBullet.h"
#include "Bonus.h"
#include "Constraint.h"
#include "GameWorld.h"
#include "PhysicsObject.h"
#include "Window.h"

#include "RenderObject.h"

#include "GameGridManager.h"
#include "InkEffectManager.h"

#include "InputKeyMap.h"
#include "SoundSource.h"
#include "Sound.h"
#include "SoundSystem.h"
#include "BulletInstanceManager.h"
#include <iostream>
#endif // x64

#ifdef _ORBIS
#include "../Common/PrefabLibrary.h"

#include "PlayerBullet.h"
#include "BulletInstanceManager.h"

#include "../Common/Constraint.h"
#include "../Common/GameWorld.h"
#include "../Common/PhysicsObject.h"
#include "../Common/RenderObject.h"

#include "InputKeyMap.h"
#include "GameGridManager.h"
#include "InkEffectManager.h"
#include<iostream>

#endif // _ORBIS
#pragma once

using namespace NCL;
using namespace CSC8503;

PlayerObject::PlayerObject(int playerID) : GameObject(), playerID(playerID), keyMap(NCL::InputKeyMap::instance()) {
	OnCollisionBeginCallback = [&](GameObject* other) {
		CollisionWith(other);
	};
#ifdef x64
	SetupAudio();
#endif // x64

	

	AttachCamera(playerID);
	gameWorld.GetCamera(playerID)->SetPlayerID(playerID);
	gameWorld.GetCamera(playerID)->SetFollow(&this->GetTransform());

}

PlayerObject::~PlayerObject() {
#ifdef x64
	alDeleteSources(1, &(*playerSource->GetSource()).source);
	alDeleteSources(1, &(*attackSource->GetSource()).source);
	SoundSystem::GetSoundSystem()->SetListener(nullptr);
	delete playerSource;
	delete attackSource;
#endif // x64

	


}

void PlayerObject::Update(float dt) {
	//Change game state
	if (health.GetHealth() <= 0) {
		ChangeLoseState();
		return;
	}

	health.Update(dt);


	lastInstancedObjects.clear();
	jumpTimer -= dt;
	projectileFireRateTimer -= dt;
	CheckGround();
	if (!isNetwork) {
		RotateToCamera();
		Vector3 dir = Vector3(0, 0, 0);
		lastKey = keyMap.GetButtonState();
		keyMap.Update();
		/*
		if (playerID == 0)		// Keyboard&Mouse user
		{
			GetInput(dir, keyMap.GetButtonState());
			Move(dir);
		}
		else {
			Vector3 movingDir;
			GetControllerInput(movingDir);
			Move(movingDir);
		}*/
		GetControllerInput(dir);
		Move(dir);
		MoveCamera();


	}

	//If on ink
	if (onGround) {
		GameNode* node = GameGridManager::instance().NearestNode(this->GetTransform().GetGlobalPosition());
		if (node)
			InkEffectManager::instance().ApplyInkEffect(node->inkType, &health, 0);
	}

}

void PlayerObject::ChangeLoseState()
{
	gameStateManager->SetGameState(GameState::Lose);
}

void PlayerObject::MoveTo(Vector3 position) {
	Vector3 diff = position - this->GetTransform().GetGlobalPosition();

	if (diff.Length() > 0.1f) {
		Vector3 dir = (position - this->GetTransform().GetGlobalPosition()).Normalised();
		this->GetPhysicsObject()->ApplyLinearImpulse(dir * moveSpeed);
	}

}

void PlayerObject::MoveByPosition(float dt, Vector3 dir)
/*
This is a temporary member function. Feel free to merge this into PlayerObject::Move when necessary.
*/
{
	this->GetTransform().SetPosition(this->GetTransform().GetGlobalPosition() + dir * 10 * dt);
}

void PlayerObject::Move(Vector3 dir) {
	this->GetPhysicsObject()->ApplyLinearImpulse(dir * moveSpeed);

	if (lastDir != Vector3(0, 0, 0)) {
		//Vector3 stopDir = dir - lastDir;
		if (NCL::InputKeyMap::instance().GetButtonState() != lastKey) {
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

void PlayerObject::GetControllerInput(Vector3& movingDir3D)		// controllerNum == 1,2,3,4
/*
This is a temporary member function used for testing controller's input. Feel free to merge this into PlayerObject::GetInput when necessary.
*/
{
	NCL::InputKeyMap& keyMap = NCL::InputKeyMap::instance();
	isFreeLook = false;

	// Thumb for movement:
	Vector3 cameraForwardDirection = gameWorld.GetCamera(playerID)->GetPosition() - this->GetTransform().GetGlobalPosition();
	Vector2 movementData{ 0,0 };
	float rightTriggerDepth = 0;
	float leftTriggerDepth = 0;
	movingDir3D = Vector3{ 0,0,0 };
	keyMap.GetAxisData(playerID, AxisInput::Axis1, movementData.x);
	if (keyMap.GetAxisData(playerID, AxisInput::Axis1, movementData.x) && keyMap.GetAxisData(playerID, AxisInput::Axis2, movementData.y))
	{
		if (!(movementData.x == 0 && movementData.y == 0))
		{
			Vector2 unitForwardDir{ 0,1 };
			float angle = atan2(unitForwardDir.x * movementData.y - movementData.x * unitForwardDir.y, unitForwardDir.x * movementData.x + unitForwardDir.y * movementData.y);
			Vector2 movingDir2D;
			movingDir2D.x = cameraForwardDirection.x * cos(angle) - (-cameraForwardDirection.z) * sin(angle);
			movingDir2D.y = (-cameraForwardDirection.z) * cos(angle) + cameraForwardDirection.x * sin(angle);
			movingDir3D = -(Vector3{ movingDir2D.x,0,-movingDir2D.y }).Normalised();
			std::cout << movingDir3D << std::endl;
		}
	}
	if (keyMap.GetAxisData(playerID, AxisInput::Axis5, rightTriggerDepth))
	{
		if (rightTriggerDepth > 0.5f)
		{
			Shoot();
		}
	}
	if (keyMap.GetAxisData(playerID, AxisInput::Axis6, leftTriggerDepth))
	{
		if (leftTriggerDepth > 0.5f && onGround && jumpTimer <= 0.0f)
		{
			Vector3 upDir = this->GetTransform().GetGlobalOrientation() * Vector3(0, 1, 0);
			jumpTimer = jumpCooldown;
			this->GetPhysicsObject()->ApplyLinearImpulse(upDir * jumpSpeed);
		}
	}
}

void PlayerObject::GetInput(Vector3& dir, unsigned int keyPress) {
	NCL::InputKeyMap& keyMap = NCL::InputKeyMap::instance();

	Vector3 fwdAxis = this->GetTransform().GetGlobalOrientation() * Vector3(0, 0, -1);

	Vector3 leftAxis = this->GetTransform().GetGlobalOrientation() * Vector3(-1, 0, 0);

	Vector3 upAxis = this->GetTransform().GetGlobalOrientation() * Vector3(0, 1, 0);

	isFreeLook = false;

	if (keyMap.CheckButtonPressed(keyPress, InputType::Foward))
	{
		dir += fwdAxis;
		//playerSource->Play(Sound::AddSound("footstep06.wav"));
	}
	if (keyMap.CheckButtonPressed(keyPress, InputType::Backward))
	{
		dir -= fwdAxis;
		//playerSource->Play(Sound::AddSound("footstep06.wav"));
	}
	if (keyMap.CheckButtonPressed(keyPress, InputType::Left))
	{
		dir += leftAxis;
		//playerSource->Play(Sound::AddSound("footstep06.wav"));
	}
	if (keyMap.CheckButtonPressed(keyPress, InputType::Right))
	{
		dir -= leftAxis;
		//playerSource->Play(Sound::AddSound("footstep06.wav"));
	}
	if (keyMap.CheckButtonPressed(keyPress, InputType::Jump) && onGround && jumpTimer <= 0.0f)
	{
		jumpTimer = jumpCooldown;
		this->GetPhysicsObject()->ApplyLinearImpulse(upAxis * jumpSpeed);
		//playerSource->Play(Sound::AddSound("swing3.wav"));
	}
	if (keyMap.CheckButtonPressed(keyPress, InputType::Action1))
	{
		Shoot();
		//attackSource->Play(Sound::AddSound("magic1.wav"));
	}
	if (keyMap.CheckButtonPressed(keyPress, InputType::FreeLook))
	{
		isFreeLook = true;
	}
	dir.Normalise();
}

void PlayerObject::CheckGround() {
	Ray r = Ray(this->GetTransform().GetGlobalPosition(), Vector3(0, -1, 0));
	RayCollision closestCollision;
	GameObject* objClosest;
	onGround = false;
	if (gameWorld.Raycast(r, closestCollision, true, this))
	{
		objClosest = (GameObject*)closestCollision.node;
		float groundDist = (closestCollision.collidedAt - this->GetTransform().GetGlobalPosition()).Length();
		//std::cout << "ground dist " << groundDist << std::endl;
		if (groundDist < jumpTriggerDist)
		{
			onGround = true;
		}
	}
}

void PlayerObject::RotateYaw(float yaw) {
	this->GetTransform().SetOrientation(Quaternion::Lerp(this->GetTransform().GetGlobalOrientation(), Quaternion::AxisAngleToQuaterion(Vector3(0, 1, 0), yaw), camTurnSpeed));
}

void PlayerObject::RotateToCamera() {
	if (!isFreeLook) {
		RotateYaw(gameWorld.GetCamera(hasCamera)->GetYaw());
	}

}

void PlayerObject::RotateByAxis() {
	NCL::InputKeyMap& keyMap = NCL::InputKeyMap::instance();
	float yaw;
	keyMap.GetAxisData(playerID, AxisInput::Axis4, yaw);
	//add yaw 
	RotateYaw(yaw * 100);

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


PlayerBullet* PlayerObject::PrepareBullet()
{
	PlayerBullet* ink = BulletInstanceManager::instance().GetPlayerBullet();
	ink->SetLifespan(projectileLifespan);
	ink->GetTransform().SetPosition(transform.GetGlobalOrientation() * projectileSpawnPoint + transform.GetGlobalPosition());
	ink->GetPhysicsObject()->SetInverseMass(2.0f);
	ink->GetPhysicsObject()->SetLinearVelocity(this->physicsObject->GetLinearVelocity() * Vector3(1, 0, 1));
	Quaternion dir = transform.GetGlobalOrientation() * Quaternion::EulerAnglesToQuaternion((float)(rand() % 100 - 50) / 20, (float)(rand() % 100 - 50) / 20, (float)(rand() % 100 - 50) / 20);
	ink->GetPhysicsObject()->ApplyLinearImpulse(dir * Vector3(0, 0, -1) * projectileForce);
	ink->SetActive(true);
	return ink;
}

void PlayerObject::Shoot() {
	if (projectileFireRateTimer > 0)
		return;
	projectileFireRateTimer = projectileFireRate;
	PlayerBullet* bullet = PrepareBullet();
}

#ifdef x64
void NCL::CSC8503::PlayerObject::SetupAudio()
{
	playerSource = new SoundSource();
	playerSource->SetPriority(SoundPriority::SOUNDPRIORITY_ALWAYS);
	playerSource->SetGain(0.0f);
	playerSource->SetSoundBuffer(Sound::AddSound("footstep06.wav"));
	playerSource->AttachSource(SoundSystem::GetSoundSystem()->GetSource());
	playerSource->SetGain(1.0f);
	playerSource->SetPitch(1.0f);

	attackSource = new SoundSource();
	attackSource->SetPriority(SoundPriority::SOUNDPRIORITY_ALWAYS);
	attackSource->SetGain(0.0f);
	attackSource->SetSoundBuffer(Sound::AddSound("magic1.wav"));
	attackSource->AttachSource(SoundSystem::GetSoundSystem()->GetSource());
	attackSource->SetGain(1.0f);
	attackSource->SetPitch(1.0f);

	SoundSystem::GetSoundSystem()->SetListener(this);

}
#endif // x64

