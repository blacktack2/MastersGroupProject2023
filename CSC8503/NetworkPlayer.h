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

			void MoveInput(unsigned int keyPress, short int axis[AxisInput::AxisInputDataMax], Vector2);

			void ServerSideMovement(float dt);

			void ClientUpdateCamera(float dt);

			void SetAxis(short int axis[AxisInput::AxisInputDataMax]);

			void GetNetworkAxis(short int axis[]);

			int GetPlayerNum() const {
				return playerID;
			}

			void Shoot();

			void SetHealth(float hp) {
				health.SetHealth(hp);
			}

			float GetYaw() {
				return yaw;
			}
			void SetYaw(float yaw) {
				this->yaw = yaw;
			}

			float GetPitch() {
				return pitch;
			}
			void SetPitch(float pitch){
				this->pitch = pitch;
			}

			bool isFrozen;
		protected:
			NetworkedGame* game;
		
		};
	}
}

