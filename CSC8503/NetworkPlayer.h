#pragma once
#include "GameObject.h"
#include "GameClient.h"

namespace NCL {
	namespace CSC8503 {
		class GameWorld;
		class NetworkedGame;

		class NetworkPlayer : public GameObject {
		public:
			NetworkPlayer(GameWorld& gameWorld, NetworkedGame* game, int num);
			~NetworkPlayer();

			void OnCollisionBegin(GameObject* otherObject) override;

			int GetPlayerNum() const {
				return playerNum;
			}

		protected:
			NetworkedGame* game;
			int playerNum;
		};
	}
}

