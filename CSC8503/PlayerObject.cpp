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

PlayerObject::PlayerObject(int id) : GameObject(), id(id), keyMap(NCL::InputKeyMap::instance()) {
	OnCollisionBeginCallback = [&](GameObject* other) {
		CollisionWith(other);
	};
	SetupAudio();

}

PlayerObject::~PlayerObject() {
	/*alDeleteSources(1, &(*playerSource->GetSource()).source);
	alDeleteSources(1, &(*attackSource->GetSource()).source);*/
	SoundSystem::GetSoundSystem()->SetListener(nullptr);
	delete playerSource;
	delete attackSource;


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
		GetInput(dir, keyMap.GetButtonState());
		Move(dir);
		MoveCamera();

		// testing (Xbox) controller input:
		Vector3 movingDir;
		GetControllerInput(1, movingDir);
		MoveByPosition(dt, movingDir);
	}

	//If on ink
	if (onGround) {
		GameNode* node = GameGridManager::instance().NearestNode(this->GetTransform().GetGlobalPosition());
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
	this->GetTransform().SetPosition(this->GetTransform().GetGlobalPosition() + dir * 20 * dt);
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

void PlayerObject::GetControllerInput(unsigned int controllerNum, Vector3& movingDir3D)		// controllerNum == 1,2,3,4
/*
This is a temporary member function used for testing controller's input. Feel free to merge this into PlayerObject::GetInput when necessary.
*/
{
	NCL::InputKeyMap& keyMap = NCL::InputKeyMap::instance();
	isFreeLook = false;
	
	// Thumb for movement:
	Vector3 cameraForwardDirection = gameWorld.GetMainCamera()->GetPosition() - this->GetTransform().GetGlobalPosition();
	Vector2 movementThumbData{ 0,0 };
	float rightTriggerDepth = 0;
	movingDir3D = Vector3{ 0,0,0 };
	if (keyMap.GetAxisData(2, AxisInput::Axis1, movementThumbData.x) && keyMap.GetAxisData(2, AxisInput::Axis2, movementThumbData.y))
	{
		if (!(movementThumbData.x == 0 && movementThumbData.y == 0))
		{
			Vector2 unitForwardDir{ 0,1 };
			float angle = atan2(unitForwardDir.x * movementThumbData.y - movementThumbData.x * unitForwardDir.y, unitForwardDir.x * movementThumbData.x + unitForwardDir.y * movementThumbData.y);
			Vector2 movingDir2D;
			movingDir2D.x = cameraForwardDirection.x * cos(angle) - (-cameraForwardDirection.z) * sin(angle);
			movingDir2D.y = (-cameraForwardDirection.z) * cos(angle) + cameraForwardDirection.x * sin(angle);
			movingDir3D = -(Vector3{ movingDir2D.x,0,-movingDir2D.y }).Normalised();
		}
	}
	if (keyMap.GetAxisData(2, AxisInput::Axis5, rightTriggerDepth))
	{
		if (rightTriggerDepth > 0.5f)
		{
			Shoot();
		}
	}
	if (keyMap.GetAxisData(2, AxisInput::Axis6, rightTriggerDepth))
	{
		if (rightTriggerDepth > 0.5f && onGround && jumpTimer <= 0.0f)
		{
			Vector3 upDir = this->GetTransform().GetGlobalOrientation() * Vector3(0, 1, 0);
			jumpTimer = jumpCooldown;
			this->GetPhysicsObject()->ApplyLinearImpulse(upDir * jumpSpeed);
		}
	}
}

void PlayerObject::GetInput(Vector3& dir, unsigned int keyPress) {
	GameObject* proj;
	NCL::InputKeyMap& keyMap = NCL::InputKeyMap::instance();

	Vector3 fwdAxis = this->GetTransform().GetGlobalOrientation() * Vector3(0, 0, -1);

	Vector3 leftAxis = this->GetTransform().GetGlobalOrientation() * Vector3(-1, 0, 0);

	Vector3 upAxis = this->GetTransform().GetGlobalOrientation() * Vector3(0, 1, 0);

	isFreeLook = false;

	if (keyMap.CheckButtonPressed(keyPress, InputType::Foward))
	{
		dir += fwdAxis;
		playerSource->Play(Sound::AddSound("footstep06.wav"));
	}
	if (keyMap.CheckButtonPressed(keyPress, InputType::Backward))
	{
		dir -= fwdAxis;
		playerSource->Play(Sound::AddSound("footstep06.wav"));
	}
	if (keyMap.CheckButtonPressed(keyPress, InputType::Left))
	{
		dir += leftAxis;
		playerSource->Play(Sound::AddSound("footstep06.wav"));
	}
	if (keyMap.CheckButtonPressed(keyPress, InputType::Right))
	{
		dir -= leftAxis;
		playerSource->Play(Sound::AddSound("footstep06.wav"));
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
		attackSource->Play(Sound::AddSound("magic1.wav"));
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
	if (hasCamera && !isFreeLook) {
		RotateYaw(gameWorld.GetMainCamera()->GetYaw());
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


PlayerBullet* PlayerObject::PrepareBullet()
{
	PlayerBullet* ink = BulletInstanceManager::instance().GetPlayerBullet();
	ink->SetLifespan(projectileLifespan);
	ink->GetTransform().SetPosition(transform.GetGlobalOrientation() * projectileSpawnPoint + transform.GetGlobalPosition());
	ink->GetPhysicsObject()->SetInverseMass(2.0f);
	ink->GetPhysicsObject()->SetLinearVelocity(this->physicsObject->GetLinearVelocity() * Vector3(1, 0, 1));
	Quaternion dir = transform.GetGlobalOrientation() * Quaternion::EulerAnglesToQuaternion((rand() % 100 - 50) / 20, (rand() % 100 - 50) / 20, (rand() % 100 - 50) / 20);
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