#pragma once
#include "GameObject.h"
#include "PhysicsObject.h"
#include "NetworkBase.h"
#include "NetworkState.h"

namespace NCL::CSC8503 {
	class GameObject;

	struct HandshakePacket : public GamePacket {
		int		objectID = -1;

		HandshakePacket() {
			type = Handshake_Message;
			size = sizeof(HandshakePacket) - sizeof(GamePacket);
		}
	};
	struct HandshakeAckPacket : public GamePacket {
		int		objectID = -1;

		HandshakeAckPacket() {
			type = Handshake_Ack;
			size = sizeof(HandshakeAckPacket) - sizeof(GamePacket);
		}
	};
	struct FullPacket : public GamePacket {
		int		objectID = -1;
		NetworkState fullState;
		int score = 0;

		FullPacket() {
			type = Full_State;
			size = sizeof(FullPacket) - sizeof(GamePacket);
		}
	};

	struct DeltaPacket : public GamePacket {
		int			fullID		= -1;
		int			objectID	= -1;
		int			pos[3];
		int			orientation[4];

		DeltaPacket() {
			type = Delta_State;
			size = sizeof(DeltaPacket) - sizeof(GamePacket);
		}
	};

	struct ClientPacket : public GamePacket {
		int		lastID;
		unsigned int buttonstates;
		int yaw;

		ClientPacket() {
			type = Received_State;
			size = sizeof(ClientPacket);
		}
	};

	class NetworkObject		{
	public:
		NetworkObject(GameObject& o, int id);
		virtual ~NetworkObject();

		//Called by clients
		virtual bool ReadPacket(GamePacket& p, float dt);
		//Called by servers
		virtual bool WritePacket(GamePacket** p, bool deltaFrame, int stateID);

		void UpdateStateHistory(int minID);

		int GetNetworkID(){
			return networkID;
		}

		void UpdateDelta(float dt);

		int smoothFrameCount = 0;
		int smoothFrameTotal = 10;

		Transform& GetRenderTransform() {
			return renderTransform;
		}

		void ServerUpdate() {
			renderTransform = object.GetTransform();
		}

	protected:
		Transform renderTransform;

		NetworkState& GetLatestNetworkState();

		bool GetNetworkState(int frameID, NetworkState& state);

		virtual bool ReadDeltaPacket(DeltaPacket &p, float dt);
		virtual bool ReadFullPacket(FullPacket &p, float dt);

		virtual bool WriteDeltaPacket(GamePacket**p, int stateID);
		virtual bool WriteFullPacket(GamePacket**p);

		GameObject& object;

		NetworkState lastFullState;

		NetworkState lastDeltaState;

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