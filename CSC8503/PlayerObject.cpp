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
#include"AnimatedRenderObject.h"
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

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
	SoundSystem::GetSoundSystem()->SetListener(nullptr);
	delete attackSource;
	delete hurtSource;
	delete jumpSource;
	delete foot1;
	delete foot2;
	delete foot3;
	ClearCamera();

}

void PlayerObject::ClearCamera() {
	camera->GetHud().ClearAndErase();
	camera->SetFollow(nullptr);
}

void PlayerObject::Update(float dt) {
	Vector3 t = transform.GetGlobalOrientation() * projectileSpawnPoint + transform.GetGlobalPosition();

	//Debug::DrawLine(transform.GetGlobalPosition() - Vector3(0, radius, 0), transform.GetGlobalPosition() - Vector3(0, jumpTriggerDist,0), Debug::RED);
	//Change game 
	if (health.GetHealth() <= 0) {
		Debug::Print("You got Inked!", Vector2(36, 50), Debug::RED,20.0f, playerID);
		AnimatedRenderObject* anim = static_cast<AnimatedRenderObject*>(GetRenderObject());
		anim->SetAnimation(AssetLibrary<MeshAnimation>::GetAsset("PlayerIdle"));
		anim->SetAniSpeed(1.0f);
		keyMap.Update();
		GetAxisInput();
		SetPitchYaw();
		MoveCamera(dt);
		//ChangeLoseState();
		return;
	}
	if (this->transform.GetGlobalPosition().y < -10) {
		health.SetHealth(0);
	}
		
	health.Update(dt);

	lastInstancedObjects.clear();
	jumpTimer -= dt;
	projectileFireRateTimer -= dt;
	CheckGround();
	if (!isNetwork) {
		Movement(dt);
	}
	else {
		if (health.GetHealth() > 0) {
			MoveAnimation();
			//Movement(dt);
		}
	}

	//If on ink
	if (onGround) {
		GameNode* node = GameGridManager::instance().NearestNode(this->GetTransform().GetGlobalPosition());
		if (node) {
			InkEffectManager::instance().ApplyInkEffect(node->inkType, &health, 0);
			if (node->inkType == NCL::InkType::BossDamage) {
				PlayAudio(PlayerAudio::HurtAudio);
			}
		}
	}
	MoveCamera(dt);
}

void PlayerObject::ChangeLoseState()
{
	gameStateManager->SetGameState(GameState::Lose);
}

void PlayerObject::SetBoundingVolume(CapsuleVolume* vol)
{
	radius = vol->GetRadius() + vol->GetHalfHeight();
	jumpTriggerDist += vol->GetRadius() + vol->GetHalfHeight();
	GameObject::SetBoundingVolume((CollisionVolume*)vol);
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

	MoveAnimation();
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

	if ((dir != Vector3(0)) && this->GetPhysicsObject()->GetLinearVelocity().y < 0.01f) {
		PlayAudio(PlayerAudio::MoveAudio);
	}
	lastDir = dir;
}

void PlayerObject::MoveCamera(float dt) {
	//rotate camera
	camera->SetPitch(pitch);
	camera->SetYaw(yaw);
	Ray ray = CollisionDetection::BuildRayFromCamera(*camera);
	RayCollision closestCollision;
	lookingAt = transform.GetGlobalPosition() + ray.GetDirection() * 30;
	if (gameWorld.Raycast(ray, closestCollision, true, this)) {
		if (closestCollision.rayDistance > 6) {
			lookingAt = closestCollision.collidedAt;
		}
		else {
			//std::cout << "dist " << closestCollision.rayDistance << std::endl;
		}
	}
	//camera->UpdateCamera(dt);
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

		const float PI = 3.1415926f;
		float thetaRad = std::acos((Vector2::Dot(unitForwardDir, movementData)) / (movementData.Length()));		// theta in [0, PI]
		float thetaDegree = (thetaRad * 180.0f) / PI;
		if (thetaDegree <= 15.0f) {
			playerMovingDirection = PlayerMovingDirection::MoveForward;
		}
		else if (thetaDegree >= 165.0f) {
			playerMovingDirection = PlayerMovingDirection::MoveForward;
		}
		else if (thetaDegree >= 75.0f && thetaDegree <= 105.0f && Vector2::Dot(Vector2{ 1,0 }, movementData) > 0.0f) {
			playerMovingDirection = PlayerMovingDirection::MoveToRight;
		}
		else if (thetaDegree >= 75.0f && thetaDegree <= 105.0f && Vector2::Dot(Vector2{ -1,0 }, movementData) > 0.0f) {
			playerMovingDirection = PlayerMovingDirection::MoveToLeft;
		}
		else if (Vector2::Dot(Vector2{ 0,1 }, movementData) > 0.0f && Vector2::Dot(Vector2{ 1,0 }, movementData) > 0.0f) {
			playerMovingDirection = PlayerMovingDirection::MoveForwardRight;
		}
		else if (Vector2::Dot(Vector2{ 0,1 }, movementData) > 0.0f && Vector2::Dot(Vector2{ -1,0 }, movementData) > 0.0f) {
			playerMovingDirection = PlayerMovingDirection::MoveForwardLeft;
		}
		else if (Vector2::Dot(Vector2{ 0,-1 }, movementData) > 0.0f && Vector2::Dot(Vector2{ 1,0 }, movementData) > 0.0f) {
			playerMovingDirection = PlayerMovingDirection::MoveBackwardRight;
		}
		else if (Vector2::Dot(Vector2{ 0,-1 }, movementData) > 0.0f && Vector2::Dot(Vector2{ -1,0 }, movementData) > 0.0f) {
			playerMovingDirection = PlayerMovingDirection::MoveBackwardLeft;
		}
	}
	else {
		playerMovingDirection = PlayerMovingDirection::Idle;
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
		if (groundDist < jumpTriggerDist)
		{
			onGround = true;
		}
	}
}

void PlayerObject::RotateYaw(float yaw) {
	//this->GetTransform().SetOrientation(Quaternion::Lerp(this->GetTransform().GetGlobalOrientation(), Quaternion::AxisAngleToQuaterion(Vector3(0, 1, 0), yaw), camTurnSpeed));
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

void NCL::CSC8503::PlayerObject::SetPitchYaw()
{
	float sensitivity = 1.5f;
	pitch += axis[Axis4] * sensitivity;
	yaw -= axis[Axis3] * sensitivity;
	pitch = std::clamp(pitch, -90.0f, 90.0f);
	yaw += (yaw < 0) ? 360.0f : ((yaw > 360.0f) ? -360.0f : 0.0f);
}

void NCL::CSC8503::PlayerObject::RotatePlayer()
{
	SetPitchYaw();

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
	Vector3 dir = lookingAt - transform.GetGlobalPosition();
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
	if (projectileFireRateTimer > 0)
		return;
	PlayAudio(PlayerAudio::ShootAudio);
	projectileFireRateTimer = projectileFireRate;
	PlayerBullet* bullet = PrepareBullet();
}

void NCL::CSC8503::PlayerObject::Jump()
{
	if (onGround && jumpTimer <= 0.0f) {
		//jumpSource->SetPosition
		PlayAudio(PlayerAudio::JumpAudio);
		Vector3 upDir = Vector3(0, 1, 0);
		jumpTimer = jumpCooldown;
		this->GetPhysicsObject()->ApplyLinearImpulse(upDir * jumpSpeed);
	}
}

void NCL::CSC8503::PlayerObject::SetupAudio()
{
	foot1 = new SoundSource();
	foot1->SetPriority(SoundPriority::SOUNDPRIORITY_ALWAYS);
	foot1->SetGain(0.0f);
	foot1->SetSoundBuffer(Sound::AddSound("fs01.wav"));
	foot1->AttachSource(SoundSystem::GetSoundSystem()->GetSource());
	foot1->SetGain(1.0f);
	foot1->SetPitch(1.0f);
	foot2 = new SoundSource();
	foot2->SetPriority(SoundPriority::SOUNDPRIORITY_ALWAYS);
	foot2->SetGain(0.0f);
	foot2->SetSoundBuffer(Sound::AddSound("fs02.wav"));
	foot2->AttachSource(SoundSystem::GetSoundSystem()->GetSource());
	foot2->SetGain(1.0f);
	foot2->SetPitch(1.0f);
	foot3 = new SoundSource();
	foot3->SetPriority(SoundPriority::SOUNDPRIORITY_ALWAYS);
	foot3->SetGain(0.0f);
	foot3->SetSoundBuffer(Sound::AddSound("fs03.wav"));
	foot3->AttachSource(SoundSystem::GetSoundSystem()->GetSource());
	foot3->SetGain(1.0f);
	foot3->SetPitch(1.0f);

	attackSource = new SoundSource();
	attackSource->SetPriority(SoundPriority::SOUNDPRIORITY_ALWAYS);
	attackSource->SetGain(0.0f);
	attackSource->SetSoundBuffer(Sound::AddSound("playerShoot.wav"));
	attackSource->AttachSource(SoundSystem::GetSoundSystem()->GetSource());
	attackSource->SetGain(1.0f);
	attackSource->SetPitch(1.0f);

	hurtSource = new SoundSource();
	hurtSource->SetPriority(SoundPriority::SOUNDPRIORITY_ALWAYS);
	hurtSource->SetGain(0.0f);
	hurtSource->SetSoundBuffer(Sound::AddSound("yell.wav"));
	hurtSource->AttachSource(SoundSystem::GetSoundSystem()->GetSource());
	hurtSource->SetGain(1.0f);
	hurtSource->SetPitch(1.0f);

	jumpSource = new SoundSource();
	jumpSource->SetPriority(SoundPriority::SOUNDPRIORITY_ALWAYS);
	jumpSource->SetGain(0.0f);
	jumpSource->SetSoundBuffer(Sound::AddSound("playerJump.wav"));
	jumpSource->AttachSource(SoundSystem::GetSoundSystem()->GetSource());
	jumpSource->SetGain(1.0f);
	jumpSource->SetPitch(1.0f);

	SoundSystem::GetSoundSystem()->SetListener(this);

}

void NCL::CSC8503::PlayerObject::PlayAudio(PlayerAudio type)
{
	srand((unsigned int)time);
	int randomFootstep = rand() % 3 + 1;
	switch (type) {
	case PlayerAudio::JumpAudio:
		jumpSource->Play(Sound::AddSound("playerJump.wav"));
		break;
	case PlayerAudio::MoveAudio:
		switch (randomFootstep) {
		case 1:foot1->Play(Sound::AddSound("fs01.wav"));
			break;
		case 2:foot2->Play(Sound::AddSound("fs02.wav"));
			break;
		case 3:foot3->Play(Sound::AddSound("fs03.wav"));
			break;
		}
		break;
	case PlayerAudio::HurtAudio:
		hurtSource->Play(Sound::AddSound("yell.wav"));
		break;
	case PlayerAudio::ShootAudio:
		attackSource->Play(Sound::AddSound("playerShoot.wav"));
		break;
	}
}

void  NCL::CSC8503::PlayerObject::MoveAnimation() {
	AnimatedRenderObject* anim = static_cast<AnimatedRenderObject*>(GetRenderObject());
	if (this->GetPhysicsObject()->GetLinearVelocity().y > 0.01f) {		// Jumping	
		if (&anim->GetAnimation() != AssetLibrary<MeshAnimation>::GetAsset("PlayerJump").get()) {
			anim->SetAnimation(AssetLibrary<MeshAnimation>::GetAsset("PlayerJump"));
			//anim->SetAnimation(AssetLibrary<MeshAnimation>::GetAsset("PlayerIdle"));
			anim->SetAniSpeed(0.35f);
		}
	}
	else if (playerMovingDirection == PlayerMovingDirection::Idle) {
		if (&anim->GetAnimation() != AssetLibrary<MeshAnimation>::GetAsset("PlayerIdle").get()) {
			anim->SetAnimation(AssetLibrary<MeshAnimation>::GetAsset("PlayerIdle"));
			anim->SetAniSpeed(1.0f);
		}
	}
	else if (playerMovingDirection == PlayerMovingDirection::MoveForward) {
		if (&anim->GetAnimation() != AssetLibrary<MeshAnimation>::GetAsset("PlayerForward").get()) {
			anim->SetAnimation(AssetLibrary<MeshAnimation>::GetAsset("PlayerForward"));
			anim->SetAniSpeed(1.0f);
		}
	}
	else if (playerMovingDirection == PlayerMovingDirection::MoveBackward) {
		if (&anim->GetAnimation() != AssetLibrary<MeshAnimation>::GetAsset("PlayerBackward").get()) {
			anim->SetAnimation(AssetLibrary<MeshAnimation>::GetAsset("PlayerBackward"));
			anim->SetAniSpeed(1.0f);
		}
	}
	else if (playerMovingDirection == PlayerMovingDirection::MoveToRight) {
		if (&anim->GetAnimation() != AssetLibrary<MeshAnimation>::GetAsset("PlayerRight").get()) {
			anim->SetAnimation(AssetLibrary<MeshAnimation>::GetAsset("PlayerRight"));
			anim->SetAniSpeed(1.0f);
		}
	}
	else if (playerMovingDirection == PlayerMovingDirection::MoveToLeft) {
		if (&anim->GetAnimation() != AssetLibrary<MeshAnimation>::GetAsset("PlayerLeft").get()) {
			anim->SetAnimation(AssetLibrary<MeshAnimation>::GetAsset("PlayerLeft"));
			anim->SetAniSpeed(1.0f);
		}
	}
	else if (playerMovingDirection == PlayerMovingDirection::MoveBackwardLeft) {
		if (&anim->GetAnimation() != AssetLibrary<MeshAnimation>::GetAsset("PlayerBackwardLeft").get()) {
			anim->SetAnimation(AssetLibrary<MeshAnimation>::GetAsset("PlayerBackwardLeft"));
			anim->SetAniSpeed(1.0f);
		}
	}
	else if (playerMovingDirection == PlayerMovingDirection::MoveBackwardRight) {
		if (&anim->GetAnimation() != AssetLibrary<MeshAnimation>::GetAsset("PlayerBackwardRight").get()) {
			anim->SetAnimation(AssetLibrary<MeshAnimation>::GetAsset("PlayerBackwardRight"));
			anim->SetAniSpeed(1.0f);
		}
	}
	else if (playerMovingDirection == PlayerMovingDirection::MoveForwardLeft) {
		if (&anim->GetAnimation() != AssetLibrary<MeshAnimation>::GetAsset("PlayerForwardLeft").get()) {
			anim->SetAnimation(AssetLibrary<MeshAnimation>::GetAsset("PlayerForwardLeft"));
			anim->SetAniSpeed(1.0f);
		}
	}
	else if (playerMovingDirection == PlayerMovingDirection::MoveForwardRight) {
		if (&anim->GetAnimation() != AssetLibrary<MeshAnimation>::GetAsset("PlayerForwardRight").get()) {
			anim->SetAnimation(AssetLibrary<MeshAnimation>::GetAsset("PlayerForwardRight"));
			anim->SetAniSpeed(1.0f);
		}
	}
}
