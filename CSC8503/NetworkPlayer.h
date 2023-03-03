#pragma once
#include "GameObject.h"
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
			void ChangeLoseState() override;

			void OnCollisionBegin(GameObject* otherObject) override;

			void RotateYaw(float yaw) {
				PlayerObject::RotateYaw(yaw);
			}

			void MoveInput(unsigned int keyPress) {
				if (health.GetHealth() > 0) {
					Vector3 dir = Vector3(0, 0, 0);
					GetInput(dir, keyPress);
					Move(dir);
					MoveCamera();
				}
				
			}

			void ServerSideMovement() {
				RotateToCamera();
				Vector3 dir = Vector3(0, 0, 0);
				lastKey = keyMap.GetButtonState();
				keyMap.Update();
				if (health.GetHealth() > 0) {
					GetInput(dir, keyMap.GetButtonState());
					Move(dir);
					MoveCamera();
				}
			}

			int GetPlayerNum() const {
				return playerID;
			}

			void Shoot();

			void SetHealth(float hp) {
				health.SetHealth(hp);
			}

			bool isFrozen;
		protected:
			NetworkedGame* game;
			int playerID;
		
		};
	}
}

