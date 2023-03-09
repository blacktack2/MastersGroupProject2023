/**
 * @file   NetworkBoss.h
 * @brief  
 * 
 * @author Felix Chiu
 * @date   March 2023
 */
#pragma once
#include "Boss.h"

namespace NCL::CSC8503 {
	class NetworkedGame;
	class NetworkBoss : public Boss {
	public:
		NetworkBoss(NetworkedGame* game);
		~NetworkBoss();

		void ChangeLoseState() override;
		void BulletModification(BossBullet* bullet);
	private:
		NetworkedGame* game;
	};
}
