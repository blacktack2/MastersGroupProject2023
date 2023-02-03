#pragma once
#include "GameObject.h"
#include "GameClient.h"
#include "PlayerObject.h"

namespace NCL {
	namespace CSC8503 {
		class GameWorld;
		class NetworkedGame;

		class NetworkPlayer : public GameObject {
		public:
			NetworkPlayer(NetworkedGame* game,int playerID);
			~NetworkPlayer();

			void OnCollisionBegin(GameObject* otherObject) override;

			int GetPlayerNum() const {
				return playerID;
			}

		protected:
			NetworkedGame* game;
			int playerID;
		};
	}
}

