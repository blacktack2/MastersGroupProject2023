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

#include "GameGridManager.h"

#include "InputKeyMap.h"
#include "SoundSource.h"
#include "Sound.h"
#include "SoundSystem.h"
#include <iostream>

using namespace NCL;
using namespace CSC8503;

PlayerObject::PlayerObject(int id) : GameObject(), id(id), keyMap(paintHell::InputKeyMap::instance()) {
	OnCollisionBeginCallback = [&](GameObject* other) {
		CollisionWith(other);
	};
	SetupAudio();
	
}

PlayerObject::~PlayerObject() {
	alDeleteSources(1, &(*playerSource->GetSource()).source);
	alDeleteSources(1, &(*attackSource->GetSource()).source);
	delete playerSource;
	delete attackSource;
		
		
}

void PlayerObject::Update(float dt) {
	if (this->GetTransform().GetGlobalPosition().y < 1.0f)		// please fix the physics system
	{
		GetTransform().SetPosition({ GetTransform().GetGlobalPosition().x, 1.0f, GetTransform().GetGlobalPosition().z });
	}

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
	}
	//temp ink
	///*
	if (onGround) {
		GameNode* node = GameGridManager::instance().NearestNode(this->GetTransform().GetGlobalPosition());
		if (node->inkType == BossDamage) {
			health.DamageOverTime(1.0f,1.0f);
		}
		//GameGridManager::instance().PaintPosition(this->GetTransform().GetGlobalPosition(), PlayerDamage);
	}
	//*/

	if (health.GetHealth() <= 0) {
		gameStateManager->SetGameState(GameState::Lose);
	}
	
}

void PlayerObject::MoveTo(Vector3 position) {
	Vector3 diff = position - this->GetTransform().GetGlobalPosition();
	
	if (diff.Length() > 0.1f) {
		Vector3 dir = (position - this->GetTransform().GetGlobalPosition()).Normalised();
		this->GetPhysicsObject()->ApplyLinearImpulse(dir * moveSpeed);
	}
		
}

void PlayerObject::Move(Vector3 dir) {
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

void PlayerObject::GetInput(Vector3& dir, unsigned int keyPress) {
	GameObject* proj;
	paintHell::InputKeyMap& keyMap = paintHell::InputKeyMap::instance();
	
	Vector3 fwdAxis = this->GetTransform().GetGlobalOrientation() * Vector3(0, 0, -1);

	Vector3 leftAxis = this->GetTransform().GetGlobalOrientation() * Vector3(-1, 0, 0);

	Vector3 upAxis = this->GetTransform().GetGlobalOrientation() * Vector3(0, 1, 0);

	isFreeLook = false;

	if (keyMap.CheckButtonPressed(keyPress, InputType::Foward))
	{
		dir += fwdAxis;
		playerSource->Play(Sound::AddSound("footstep06.wav"));
	}
	if (keyMap.CheckButtonPressed(keyPress, InputType::Backward) )
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
	if (keyMap.CheckButtonPressed(keyPress,InputType::Jump) && onGround && jumpTimer <= 0.0f ) 
	{
		jumpTimer = jumpCooldown;
		this->GetPhysicsObject()->ApplyLinearImpulse(upAxis * jumpSpeed);
		//playerSource->Play(Sound::AddSound("swing3.wav"));
	}
	if (keyMap.CheckButtonPressed(keyPress,InputType::Action1)) 
	{
		Shoot();
		attackSource->Play(Sound::AddSound("magic1.wav"));
	}
	if (keyMap.CheckButtonPressed(keyPress,InputType::FreeLook))
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


void PlayerObject::Shoot() {
	if (projectileFireRateTimer > 0)
		return;
	projectileFireRateTimer = projectileFireRate;
	Bullet* ink = new Bullet(*(Bullet*)AssetLibrary::GetPrefab("bullet"));
	ink->SetLifespan(projectileLifespan);
	ink->GetTransform().SetPosition(transform.GetGlobalOrientation() * projectileSpawnPoint + transform.GetGlobalPosition());
	ink->GetPhysicsObject()->SetInverseMass(2.0f);
	ink->GetPhysicsObject()->SetLinearVelocity(this->physicsObject->GetLinearVelocity() * Vector3(1, 0, 1));
	ink->GetPhysicsObject()->ApplyLinearImpulse(transform.GetGlobalOrientation() * Vector3(0, 0, -1) * projectileForce);
	gameWorld.AddGameObject(ink);
	ink->OnCollisionBeginCallback = [ink](GameObject* other) {
		GameGridManager::instance().PaintPosition(ink->GetTransform().GetGlobalPosition(), PlayerDamage);
		ink->Delete();
	};
	lastInstancedObjects.push_back(ink);
}

void NCL::CSC8503::PlayerObject::SetupAudio()
{
	playerSource = new SoundSource();
	playerSource->SetPriority(SoundPriority::SOUNDPRIORITY_HIGH);
	playerSource->SetGain(0.0f);
	playerSource->SetSoundBuffer(Sound::AddSound("footstep06.wav"));
	playerSource->AttachSource(SoundSystem::GetSoundSystem()->GetSource());
	playerSource->SetGain(1.0f);
	playerSource->SetPitch(1.0f);

	attackSource = new SoundSource();
	attackSource->SetPriority(SoundPriority::SOUNDPRIORITY_HIGH);
	attackSource->SetGain(0.0f);
	attackSource->SetSoundBuffer(Sound::AddSound("magic1.wav"));
	attackSource->AttachSource(SoundSystem::GetSoundSystem()->GetSource());
	attackSource->SetGain(1.0f);
	attackSource->SetPitch(1.0f);

	SoundSystem::GetSoundSystem()->SetListener(this);

}
