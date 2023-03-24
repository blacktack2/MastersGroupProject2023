/**
 * @file   NetworkBoss.h
 * @brief  
 * 
 * @author Felix Chiu
 * @date   March 2023
 */
#pragma once
#include "Boss.h"

namespace NCL {

	namespace CSC8503 {
		class NetworkedGame;
		class NetworkBoss : public Boss {
		public:
			NetworkBoss(NetworkedGame* game);
			~NetworkBoss();

			void ChangeLoseState() override;
			void BulletModification(BossBullet* bullet);
			void SetBossAction(Boss::BossAction action);
		private:
			NetworkedGame* game;
		};
	}
}
