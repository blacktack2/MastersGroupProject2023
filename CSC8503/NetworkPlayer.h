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

			void MoveInput(unsigned int keyPress, short int axis[AxisInput::AxisInputDataMax]);

			void ServerSideMovement();

			void SetAxis(short int axis[AxisInput::AxisInputDataMax]);

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
		
		};
	}
}

