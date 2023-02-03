#pragma once
#include "NetworkBase.h"
namespace NCL {
	namespace CSC8503 {
		class GameWorld;
		class GameServer : public NetworkBase {
		public:
			GameServer(int onPort, int maxClients);
			~GameServer();

			bool Initialise();
			void Shutdown();

			void SetGameWorld(GameWorld &g);

			bool SendGlobalPacket(int msgID, bool isReliable = false);
			bool SendGlobalPacket(GamePacket* packet, bool isReliable = false);

			bool SendPacket(GamePacket* payload, int clientID, bool isReliable = false);

			virtual void UpdateServer();

			std::map<int, bool> handshakeMap;

			std::vector <int> GetClientIDs() {
				return clientIDs;
			}

		protected:
			int			port;
			int			clientMax;
			int			clientCount;
			int			nextClientID = 1;
			GameWorld*	gameWorld;

			int incomingDataRate;
			int outgoingDataRate;

			std::map<int, _ENetPeer*> clientIDToPeer;
			std::map<_ENetPeer*, int> peerToClientID;
			std::vector <int> clientIDs;
			//std::map<_ENetPeer*, ID> peerMap;
		};
	}
}
