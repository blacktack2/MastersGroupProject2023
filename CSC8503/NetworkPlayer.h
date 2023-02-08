#pragma once
#include "GameObject.h"
#include "GameClient.h"
#include "PlayerObject.h"

namespace NCL {
	namespace CSC8503 {
		class GameWorld;
		class NetworkedGame;

		class NetworkPlayer : public PlayerObject {
		public:
			NetworkPlayer(NetworkedGame* game,int playerID);
			~NetworkPlayer();

			void Update(float dt);
			void OnCollisionBegin(GameObject* otherObject) override;

			void RotateYaw(float yaw) {
				PlayerObject::RotateYaw(yaw);
			}

			void MoveInput(unsigned int keyPress) {
				Vector3 dir = Vector3(0, 0, 0);
				GetInput(dir, keyPress);
				Move(dir);
				MoveCamera();
			}

			void Test() {
				RotateToCamera();
				Vector3 dir = Vector3(0, 0, 0);
				lastKey = keyMap.GetButtonState();
				keyMap.Update();
				GetInput(dir, keyMap.GetButtonState());
				Move(dir);
				MoveCamera();
			}

			int GetPlayerNum() const {
				return playerID;
			}

		protected:
			NetworkedGame* game;
			int playerID;
		};
	}
}

