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
	if(!this->game->GetServer())
		isSpawnBullet = false;
}

NetworkBoss::~NetworkBoss()
{
}

void NetworkBoss::ChangeLoseState()
{
	GameStatePacket packet;
	packet.state = GameState::Win;

	if (game->GetServer())
		game->GetServer()->SendGlobalPacket(static_cast<GamePacket*>(&packet));
	Boss::ChangeLoseState();
}

void NetworkBoss::BulletModification(BossBullet* bullet)
{
	ItemInitPacket newObj;
	GameServer* server = game->GetServer();
	bullet->OnDestroyCallback = [&, bullet, server](Bullet& b) {
		if (server) {
			ItemDestroyPacket packet;
			packet.position = bullet->GetTransform().GetGlobalPosition();
			packet.objectID = bullet->GetNetworkObject()->GetNetworkID();
			server->SendGlobalPacket(&packet, true);
		}
	};

	Transform objTransform = bullet->GetTransform();
	newObj.position = objTransform.GetGlobalPosition();
	newObj.orientation = objTransform.GetGlobalOrientation();
	newObj.scale = objTransform.GetScale();
	newObj.velocity = bullet->GetPhysicsObject()->GetLinearVelocity();
	newObj.angular = bullet->GetPhysicsObject()->GetAngularVelocity();
	newObj.objectID = bullet->GetNetworkObject()->GetNetworkID();
	newObj.paintRadius = bullet->GetPaintRadius() * 100;

	if (game->GetServer())
		game->GetServer()->SendGlobalPacket(&newObj);
}

void NetworkBoss::SetBossAction(BossAction action)
{
	bossAction = action;
}
