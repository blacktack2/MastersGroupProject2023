#include "NetworkPlayer.h"
#include "NetworkedGame.h"

using namespace NCL;
using namespace CSC8503;

NetworkPlayer::NetworkPlayer(NetworkedGame* game, int playerID):PlayerObject(playerID) {
	this->game = game;
	this->playerID = playerID;
	this->isNetwork = true;
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