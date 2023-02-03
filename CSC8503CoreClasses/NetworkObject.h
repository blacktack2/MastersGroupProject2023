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
		short int	orientation[4];

		DeltaPacket() {
			type = Delta_State;
			size = sizeof(DeltaPacket) - sizeof(GamePacket);
		}
	};

	struct ClientPacket : public GamePacket {
		int		lastID;
		unsigned int buttonstates;
		unsigned short int yaw;

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

	protected:

		NetworkState& GetLatestNetworkState();

		bool GetNetworkState(int frameID, NetworkState& state);

		virtual bool ReadDeltaPacket(DeltaPacket &p, float dt);
		virtual bool ReadFullPacket(FullPacket &p, float dt);

		virtual bool WriteDeltaPacket(GamePacket**p, int stateID);
		virtual bool WriteFullPacket(GamePacket**p);

		GameObject& object;

		NetworkState lastFullState;

		std::vector<NetworkState> stateHistory;

		int deltaErrors;
		int fullErrors;

		int networkID;
	};
}