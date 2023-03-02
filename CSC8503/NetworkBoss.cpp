/**
 * @file   NetworkBoss.cpp
 * @brief
 *
 * @author Felix Chiu
 * @date   March 2023
 */
#pragma once
#include "NetworkedGame.h"
#include "NetworkBoss.h"
#include "NetworkObject.h"
#include "GameServer.h"

NetworkBoss::NetworkBoss(NetworkedGame* game) : Boss()
{
	this->game = game;
}

NetworkBoss::~NetworkBoss()
{
}

void NetworkBoss::BulletModification(BossBullet* bullet)
{
	ItemInitPacket newObj;
	Transform objTransform = bullet->GetTransform();
	newObj.position = objTransform.GetGlobalPosition();
	newObj.orientation = objTransform.GetGlobalOrientation();
	newObj.scale = objTransform.GetScale();
	newObj.velocity = bullet->GetPhysicsObject()->GetLinearVelocity();
	newObj.objectID = bullet->GetNetworkObject()->GetNetworkID();

	if (game->GetServer())
		game->GetServer()->SendGlobalPacket(&newObj);
}

void NetworkBoss::ChangeLoseState()
{
	GameStatePacket packet;
	packet.state = GameState::Win;
	

	if (game->GetServer())
		game->GetServer()->SendGlobalPacket(static_cast<GamePacket*>(&packet));
}
