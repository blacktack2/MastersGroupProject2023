#include "NetworkPlayer.h"
#include "NetworkedGame.h"
#include "NetworkObject.h"
#include "GameServer.h"

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

void NetworkPlayer::Shoot()
{
	if (projectileFireRateTimer > 0)
		return;
	projectileFireRateTimer = projectileFireRate;
	PlayerBullet* bullet = PrepareBullet();

	ItemInitPacket newObj;
	Transform objTransform = bullet->GetTransform();
	newObj.position = objTransform.GetGlobalPosition();
	newObj.orientation = objTransform.GetGlobalOrientation();
	newObj.scale = objTransform.GetScale();
	newObj.velocity = bullet->GetPhysicsObject()->GetLinearVelocity();
	newObj.objectID = bullet->GetNetworkObject()->GetNetworkID();
	
	if(game->GetServer())
		game->GetServer()->SendGlobalPacket(&newObj);
}
