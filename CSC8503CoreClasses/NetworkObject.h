#pragma once
#include "GameObject.h"
#include "PhysicsObject.h"
#include "NetworkBase.h"
#include "NetworkState.h"
#include "InputKeyMap.h"

namespace NCL {
	namespace CSC8503 {
		class GameObject;

		struct ItemInitPacket : public GamePacket {
			int		objectID = -1;
			short int index = 0;
			Vector3		position;
			Vector3		scale;
			Quaternion	orientation;
			Vector3		velocity;
			Vector3		angular;
			short int	paintRadius = 200;

			ItemInitPacket() {
				type = Item_Init_Message;
				size = sizeof(ItemInitPacket) - sizeof(GamePacket);
			}
		};
		struct ItemDestroyPacket : public GamePacket {
			int		objectID = -1;
			short int index = 0;
			Vector3		position;

			ItemDestroyPacket() {
				type = Item_Destroy_Message;
				size = sizeof(ItemDestroyPacket) - sizeof(GamePacket);
			}
		};

		struct FullPacket : public GamePacket {
			int		objectID = -1;
			NetworkState fullState;
			int health = -1;

			FullPacket() {
				type = Full_State;
				size = sizeof(FullPacket) - sizeof(GamePacket);
			}
		};

		struct DeltaPacket : public GamePacket {
			int			fullID = -1;
			int			objectID = -1;
			int			pos[3] = { 0 };
			int			orientation[4] = { 0 };

			DeltaPacket() {
				type = Delta_State;
				size = sizeof(DeltaPacket) - sizeof(GamePacket);
			}
		};

		struct ClientPacket : public GamePacket {
			int		lastID = -1;
			unsigned int buttonstates = 0;
			short int axis[AxisInput::AxisInputDataMax] = { 0 };
			int yaw = 0;
			int pitch = 0;

			ClientPacket() {
				type = Received_State;
				size = sizeof(ClientPacket);
			}
		};

		class NetworkObject {
		public:
			NetworkObject(GameObject& o, int id = -1);
			virtual ~NetworkObject();

			//Called by clients
			virtual bool ReadPacket(GamePacket& p, float dt);
			//Called by servers
			virtual bool WritePacket(GamePacket** p, bool deltaFrame, int stateID);

			void UpdateStateHistory(int minID);

			int GetNetworkID() {
				return networkID;
			}

			void UpdateDelta(float dt);

			int smoothFrameCount = 0;
			int smoothFrameTotal = 10;

			Transform& GetRenderTransform() {
				return renderTransform;
			}

			void SnapRenderToSelf() {
				renderTransform = object.GetTransform();
			}

			bool isActive() {
				return object.IsActive();
			}

		protected:
			Transform renderTransform;

			NetworkState& GetLatestNetworkState();

			bool GetNetworkState(int frameID, NetworkState& state);

			virtual bool ReadDeltaPacket(DeltaPacket& p, float dt);
			virtual bool ReadFullPacket(FullPacket& p, float dt);
			virtual bool ReadItemInitPacket(ItemInitPacket& p, float dt);
			virtual bool ReadItemDestroyPacket(ItemDestroyPacket& p, float dt);

			virtual bool WriteDeltaPacket(GamePacket** p, int stateID);
			virtual bool WriteFullPacket(GamePacket** p);

			GameObject& object;

			NetworkState lastFullState;

			NetworkState lastDeltaState = NetworkState();

			std::vector<NetworkState> stateHistory;

			int deltaErrors;
			int fullErrors;

			int networkID;

			float deltaOrientationDivisor = 10000000;
			float deltaOrientationDivisorInverse = 1 / deltaOrientationDivisor;

			float deltaPosDivisor = 1000;
			float deltaPosDivisorInverse = 1 / deltaPosDivisor;

			float smoothFrameInverse = 1.0f / smoothFrameTotal;
		};
	}
}