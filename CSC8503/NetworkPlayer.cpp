#include "NetworkPlayer.h"
#include "NetworkedGame.h"
#include "NetworkObject.h"
#include "GameServer.h"
#include "Sound.h"
using namespace NCL;
using namespace CSC8503;

NetworkPlayer::NetworkPlayer(NetworkedGame* game, int playerID):PlayerObject(playerID) {
	this->game = game;
	this->playerID = playerID;
	this->isNetwork = true;
	isFrozen = false;
}

NetworkPlayer::~NetworkPlayer() {

}

void NetworkPlayer::Update(float dt) {
	PlayerObject::Update(dt);
}

void NetworkPlayer::ChangeLoseState() {

}

void NetworkPlayer::OnCollisionBegin(GameObject* otherObject) {
	if (game) {
		if (dynamic_cast<NetworkPlayer*>(otherObject))
		{
			game->OnPlayerCollision(this, (NetworkPlayer*)otherObject);
		}
	}
}

void NCL::CSC8503::NetworkPlayer::MoveInput(unsigned int keyPress, short int axis[AxisInput::AxisInputDataMax], Vector2 rotationAxis)
{
	if (health.GetHealth() > 0) {
		Vector3 dir = Vector3(0, 0, 0);
		SetAxis(axis);
		GetDir(dir);
		GetButtonInput(keyPress);
		Move(dir);
		this->yaw = rotationAxis.x;
		this->pitch = rotationAxis.y;
		RotateYaw(yaw);
		MoveCamera(0.05f);
		MoveAnimation();
	}
}

void NCL::CSC8503::NetworkPlayer::ServerSideMovement(float dt)
{
	//RotateToCamera();
	Vector3 dir = Vector3(0, 0, 0);
	lastKey = keyMap.GetButtonState();
	keyMap.Update();
	if (health.GetHealth() > 0) {
		RotatePlayer();
		MoveCamera(dt);

		GetAxisInput();
		GetDir(dir);

		GetButtonInput(keyMap.GetButtonState());
		Move(dir);
		MoveAnimation();
		RotateToCamera();
	}
}

void NCL::CSC8503::NetworkPlayer::ClientUpdateCamera(float dt)
{
	keyMap.Update();
	RotatePlayer();
	MoveCamera(dt);
}

void NCL::CSC8503::NetworkPlayer::SetAxis(short int axis[AxisInput::AxisInputDataMax])
{
	for (int i = 0; i < AxisInput::AxisInputDataMax; i++) {
		float input;
		input = static_cast<float>(axis[i] / 10000);
		this->axis[i] = input;
	}
}

void NCL::CSC8503::NetworkPlayer::GetNetworkAxis(short int axis[])
{
	GetAxisInput();
	for (int i = 0; i < AxisInput::AxisInputDataMax; i++) {
		axis[i] = static_cast<short int>(this->axis[i] * 10000);
	}
}

void NetworkPlayer::Shoot()
{
	if (projectileFireRateTimer > 0)
		return;
	PlayAudio(PlayerAudio::ShootAudio);
	projectileFireRateTimer = projectileFireRate;
	PlayerBullet* bullet = PrepareBullet();
	GameServer* server = game->GetServer();
	bullet->OnDestroyCallback = [&, bullet, server](Bullet& b) {
		if (server) {
			ItemDestroyPacket packet;
			packet.position = bullet->GetTransform().GetGlobalPosition();
			packet.objectID = bullet->GetNetworkObject()->GetNetworkID();
			server->SendGlobalPacket(&packet, true);
		}
	};

	ItemInitPacket newObj;
	Transform objTransform = bullet->GetTransform();
	newObj.position = objTransform.GetGlobalPosition();
	newObj.orientation = objTransform.GetGlobalOrientation();
	newObj.scale = objTransform.GetScale();
	newObj.velocity = bullet->GetPhysicsObject()->GetLinearVelocity();
	newObj.angular = bullet->GetPhysicsObject()->GetAngularVelocity();
	newObj.objectID = bullet->GetNetworkObject()->GetNetworkID();
	newObj.paintRadius = bullet->GetPaintRadius() * 100;

	
	if(game->GetServer())
		game->GetServer()->SendGlobalPacket(&newObj,true);
}

void NetworkPlayer::PlayShootSound()
{
	PlayAudio(PlayerAudio::ShootAudio);
}
