#include "NetworkPlayer.h"

#include "GameWorld.h"
#include "NetworkedGame.h"

using namespace NCL;
using namespace CSC8503;

NetworkPlayer::NetworkPlayer(GameWorld& gameWorld, NetworkedGame* game, int num) : GameObject(gameWorld) {
	this->game = game;
	playerNum  = num;
}

NetworkPlayer::~NetworkPlayer()	{

}

void NetworkPlayer::OnCollisionBegin(GameObject* otherObject) {
	if (game) {
		if (dynamic_cast<NetworkPlayer*>(otherObject))
		{
			game->OnPlayerCollision(this, (NetworkPlayer*)otherObject);
		}
	}
}