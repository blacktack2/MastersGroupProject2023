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
			NetworkedGame();
			~NetworkedGame();

			void StartAsServer();
			void StartAsClient(char a, char b, char c, char d);

			void UpdateGame(float dt) override;

			GameObject* SpawnPlayer(int playerID, bool isSelf = false);

			void StartLevel();

			void ReceivePacket(int type, GamePacket* payload, int source) override;

			void OnPlayerCollision(NetworkPlayer* a, NetworkPlayer* b);

			void SetName(string name) {
				this->name = name;
			}
			string GetName() {
				return this->name;
			}

		protected:
			void UpdateAsServer(float dt);
			void UpdateAsClient(float dt);

			void BroadcastSnapshot(bool deltaFrame);
			void SendSnapshot(bool deltaFrame,int playerID);
			void UpdateMinimumState();

			void ClientProcessNetworkObject(GamePacket* payload, int objID);
			void ServerProcessNetworkObject(GamePacket* payload, int playerID);

			void PlayerJoined(int playerID);
			void PlayerLeft(int playerID);

			PlayerObject* AddNetworkPlayerToWorld(const Vector3& position, bool cameraFollow, int playerID);

			std::map<int, int> stateIDs;

			GameServer* thisServer;
			GameClient* thisClient;
			float timeToNextPacket;
			float game_dt;
			int packetsToSnapshot;

			int stateID;
			int selfID;

			std::vector<NetworkObject*> networkObjects;
			//std::map<int, NetworkObject*> networkObjects;

			std::map<int, GameObject*> serverPlayers;
			GameObject* localPlayer;
			string name;
		};
	}
}

