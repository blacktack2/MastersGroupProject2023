#pragma once
#include "PlayerObject.h"

#include "AssetLibrary.h"
#include "PrefabLibrary.h"

#include "PlayerBullet.h"
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

using namespace NCL;
using namespace CSC8503;

PlayerObject::PlayerObject(int playerID) : GameObject(), playerID(playerID), keyMap(NCL::InputKeyMap::instance()), camera(gameWorld.GetCamera(playerID)) {
	OnCollisionBeginCallback = [&](GameObject* other) {
		CollisionWith(other);
	};
	SetupAudio();

	AttachCamera(playerID);
	camera->SetPlayerID(playerID);
	camera->SetFollow(&this->GetTransform());

}

PlayerObject::~PlayerObject() {
	alDeleteSources(1, &(*playerSource->GetSource()).source);
	alDeleteSources(1, &(*attackSource->GetSource()).source);
	SoundSystem::GetSoundSystem()->SetListener(nullptr);
	delete playerSource;
	delete attackSource;


}

void PlayerObject::Update(float dt) {
	//Change game state
	MoveCamera(dt);
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
		Movement(dt);
	}

	//If on ink
	if (onGround) {
		GameNode* node = GameGridManager::instance().NearestNode(this->GetTransform().GetGlobalPosition());
		if(node)
			InkEffectManager::instance().ApplyInkEffect(node->inkType, &health, 0);
	}
	MoveCamera(dt);
}

void PlayerObject::ChangeLoseState()
{
	gameStateManager->SetGameState(GameState::Lose);
}

void NCL::CSC8503::PlayerObject::Movement(float dt)
{
	isFreeLook = false;
	Vector3 dir = Vector3(0, 0, 0);
	lastKey = keyMap.GetButtonState();
	keyMap.Update();

	GetAxisInput();
	GetDir(dir);

	GetButtonInput(keyMap.GetButtonState());
	Move(dir);

	RotatePlayer();
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
	if(dir != Vector3(0)){
		playerSource->Play(Sound::AddSound("footstep06.wav"));
	}
	lastDir = dir;
}

void PlayerObject::MoveCamera(float dt) {
	//rotate camera
	camera->SetPitch(pitch);
	camera->SetYaw(yaw);
	camera->UpdateCamera(dt);
}

void PlayerObject::GetAxisInput()
{

	NCL::InputKeyMap& keyMap = NCL::InputKeyMap::instance();
	for (int i = 0; i < AxisInput::AxisInputDataMax; i++) {
		float input = 0;
		keyMap.GetAxisData(playerID, static_cast<AxisInput>(i), input);
		axis[i] = input;
	}
}

void NCL::CSC8503::PlayerObject::GetDir(Vector3& movingDir3D)
{
	NCL::InputKeyMap& keyMap = NCL::InputKeyMap::instance();
	
	// Thumb for movement:
	Vector3 cameraForwardDirection = camera->GetPosition() - this->GetTransform().GetGlobalPosition();
	Vector2 movementData{ axis[Axis1], axis[Axis2] };
	float rightTriggerDepth = axis[Axis5];
	float leftTriggerDepth = axis[Axis6];
	movingDir3D = Vector3{ 0,0,0 };
	
	if (!(movementData.x == 0 && movementData.y == 0))
	{
		Vector2 unitForwardDir{ 0,1 };
		float angle = atan2(unitForwardDir.x * movementData.y - movementData.x * unitForwardDir.y, unitForwardDir.x * movementData.x + unitForwardDir.y * movementData.y);
		Vector2 movingDir2D;
		movingDir2D.x = cameraForwardDirection.x * cos(angle) - (-cameraForwardDirection.z) * sin(angle);
		movingDir2D.y = (-cameraForwardDirection.z) * cos(angle) + cameraForwardDirection.x * sin(angle);
		movingDir3D = -(Vector3{ movingDir2D.x,0,-movingDir2D.y }).Normalised();
	}
	if (rightTriggerDepth > 0.5f)
	{
		Shoot();
	}

	if (leftTriggerDepth > 0.5f)
	{
		Jump();
	}
}


void PlayerObject::GetButtonInput(unsigned int keyPress) {
	isFreeLook = false;
	if (keyMap.CheckButtonPressed(keyPress, InputType::Jump, playerID))
	{
		Jump();
	}
	if (keyMap.CheckButtonPressed(keyPress, InputType::Action1, playerID))
	{
		Shoot();
	}
	if (keyMap.CheckButtonPressed(keyPress, InputType::FreeLook, playerID))
	{
		isFreeLook = true;
	}
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

void NCL::CSC8503::PlayerObject::RotatePlayer()
{
	float sensitivity = 1.5f;
	pitch += axis[Axis4] * sensitivity;
	yaw -= axis[Axis3] * sensitivity;
	pitch = std::clamp(pitch, -90.0f, 90.0f);
	yaw += (yaw < 0) ? 360.0f : ((yaw > 360.0f) ? -360.0f : 0.0f);

	RotateYaw(yaw);
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
	Ray ray = CollisionDetection::BuildRayFromCamera(*camera);
	Vector3 dir;
	RayCollision closestCollision;
	if (gameWorld.Raycast(ray, closestCollision, true, this)) {
		dir = closestCollision.collidedAt - transform.GetGlobalPosition();
		lookingAt = closestCollision.collidedAt;
	}
	else {
		dir = ray.GetDirection() * 100;
		lookingAt = closestCollision.collidedAt;
	}
	dir = Quaternion::EulerAnglesToQuaternion((float)(rand() % 100 - 50) / 20, (float)(rand() % 100 - 50) / 20, (float)(rand() % 100 - 50) / 20) * dir; // add randomness

	PlayerBullet* ink = BulletInstanceManager::instance().GetPlayerBullet();
	ink->SetLifespan(projectileLifespan);
	ink->GetTransform().SetPosition(transform.GetGlobalOrientation() * projectileSpawnPoint + transform.GetGlobalPosition());
	ink->GetPhysicsObject()->SetInverseMass(2.0f);
	ink->GetPhysicsObject()->SetLinearVelocity(this->physicsObject->GetLinearVelocity() * Vector3(1, 0, 1));
	/*
	Quaternion dir = transform.GetGlobalOrientation() * Quaternion::AxisAngleToQuaterion(Vector3(1, 0, 0), pitch);
	dir = dir * Quaternion::EulerAnglesToQuaternion((float)(rand() % 100 - 50) / 20, (float)(rand() % 100 - 50) / 20, (float)(rand() % 100 - 50) / 20); // add randomness
	ink->GetPhysicsObject()->ApplyLinearImpulse(dir * Vector3(0, 0, -1) * projectileForce);
	*/
	ink->GetPhysicsObject()->ApplyLinearImpulse(dir.Normalised() * projectileForce);
	ink->SetActive(true);
	return ink;
}

void PlayerObject::Shoot() {
	attackSource->Play(Sound::AddSound("magic1.wav"));
	if (projectileFireRateTimer > 0)
		return;
	projectileFireRateTimer = projectileFireRate;
	PlayerBullet* bullet = PrepareBullet();
}

void NCL::CSC8503::PlayerObject::Jump()
{
	if (onGround && jumpTimer <= 0.0f) {
		Vector3 upDir = this->GetTransform().GetGlobalOrientation() * Vector3(0, 1, 0);
		jumpTimer = jumpCooldown;
		this->GetPhysicsObject()->ApplyLinearImpulse(upDir * jumpSpeed);
	}
}


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