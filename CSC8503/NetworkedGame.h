#pragma once
#include "TutorialGame.h"
#include "NetworkBase.h"
#include "InputKeyMap.h"

namespace NCL {
	namespace CSC8503 {
		class GameServer;
		class GameClient;
		class NetworkPlayer;

		class NetworkedGame : public TutorialGame, public PacketReceiver {
		public:
			NetworkedGame(bool isServer = true);
			~NetworkedGame();

			void StartAsServer();
			void StartAsClient(char a, char b, char c, char d);

			void UpdateGame(float dt) override;

			GameObject* SpawnPlayer(int playerID, bool isSelf = false);

			void StartLevel() override;

			void ReceivePacket(int type, GamePacket* payload, int source) override;

			void OnPlayerCollision(NetworkPlayer* a, NetworkPlayer* b);

			void SetName(std::string name) {
				this->name = name;
			}
			std::string GetName() {
				return this->name;
			}

			void FreezeSelf();
			void UnfreezeSelf();

		protected:
			void UpdateAsServer(float dt);
			void UpdateAsClient(float dt);

			void BroadcastSnapshot(bool deltaFrame);
			void SendSnapshot(bool deltaFrame,int playerID);
			void UpdateMinimumState();

			void ClientProcessNetworkObject(GamePacket* payload, int objID);
			void ServerProcessNetworkObject(GamePacket* payload, int playerID);

			void PlayerJoinedServer(int playerID);
			void PlayerLeftServer(int playerID);

			void ServerGetInstantiatedObject(NetworkPlayer* player);
			void SendInitItemPacket(GameObject* obj);

			//packet handle
			void HandleDeltaPacket(GamePacket* payload, int source);
			void HandleFullPacket(GamePacket* payload, int source);
			void HandlePlayerConnectedPacket(GamePacket* payload, int source);
			void HandlePlayerDisconnectedPacket(GamePacket* payload, int source);
			void HandleHandshakePacket(GamePacket* payload, int source);
			void HandleItemInitPacket(GamePacket* payload, int source);
			void HandleBossActionPacket(GamePacket* payload, int source);


			PlayerObject* AddNetworkPlayerToWorld(const Vector3& position, bool cameraFollow, int playerID);

			std::map<int, int> stateIDs;

			GameServer* thisServer;
			GameClient* thisClient;
			float timeToNextPacket;
			float game_dt;
			int packetsToSnapshot;

			int stateID;
			int selfID;

			int objectID;

			std::map<int, GameObject*> serverPlayers;
			GameObject* localPlayer;
			std::string name;

			float packetGap = 1.0f / 120.0f; //120hz server/client update
			int fullPacketToDeltaRate = 30;
		};
	}
}

