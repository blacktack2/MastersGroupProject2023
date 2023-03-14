/**
 * @file   NetworkGame.cpp
 * @brief  A class for networked multiplayer game
 *
 * @author Rich Davidson
 * @author Felix Chiu
 * @date   Dec 2022
 */
#pragma once
#include "TutorialGame.h"
#include "NetworkBase.h"
#include "InputKeyMap.h"
#include "NetworkBoss.h"

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
			void Clear() override;
			void StartLobby();
			void StartLevel() override;
			void SpawnPlayers();

			void BroadcastGameStateChange();

			void UpdateGame(float dt) override;

			PlayerObject* SpawnPlayer(int playerID, bool isSelf = false);

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

			GameServer* GetServer();

			void Disconnect();

		protected:
			void UpdateAsServer(float dt);
			void UpdateAsClient(float dt);

			void BroadcastSnapshot(bool deltaFrame);
			void SendSnapshot(bool deltaFrame,int playerID);
			void UpdateMinimumState();

			void ClientProcessNetworkObject(GamePacket* payload, int objID);
			void ServerProcessNetworkObject(GamePacket* payload, int playerID);

			PlayerObject* PlayerJoinedServer(int playerID);
			void PlayerLeftServer(int playerID);

			//packet handle
			void HandleDeltaPacket(GamePacket* payload, int source);
			void HandleFullPacket(GamePacket* payload, int source);
			void HandlePlayerConnectedPacket(GamePacket* payload, int source);
			void HandlePlayerDisconnectedPacket(GamePacket* payload, int source);
			void HandlePlayerSyncPacket(GamePacket* payload, int source);
			void HandleItemInitPacket(GamePacket* payload, int source);
			void HandleBossActionPacket(GamePacket* payload, int source);
			void HandleGameStatePacket(GamePacket* payload, int source);
			void HandleLobbyPacket(GamePacket* payload, int source);

			PlayerObject* AddNetworkPlayerToWorld(const Vector3& position, int playerID);
			NetworkBoss* AddNetworkBossToWorld(const Vector3& position, Vector3 dimensions, float inverseMass);
			
			void UpdateHud(float dt) override;
			void ProcessState() override;

			std::map<int, int> stateIDs;

			GameServer* thisServer;
			GameClient* thisClient;
			float timeToNextPacket;
			float game_dt;
			int packetsToSnapshot;

			int stateID;
			int selfID;

			int objectID;

			std::vector<int>connectedPlayerIDs;

			std::map<int, GameObject*> serverPlayers;
			PlayerObject* localPlayer;
			std::string name;

			float packetGap = 1.0f / 120.0f; //120hz server/client update
			int fullPacketToDeltaRate = 30;

			bool canJoin;
		};


		enum class LobbyState {
			Lobby,
			Full,
			Started
		};

		struct MessagePacket : public GamePacket {
			short playerID;
			short messageID;

			MessagePacket() {
				type = Message;
				size = sizeof(short) * 2;
			}
		};
		struct GameStatePacket : public GamePacket {
			GameState state;

			GameStatePacket() {
				type = GameState_Message;
				size = sizeof(GameStatePacket) - sizeof(GamePacket);
			}
			~GameStatePacket() {}
		};
		struct PlayerSyncPacket : public GamePacket {
			int		objectID = -1;

			PlayerSyncPacket() {
				type = PlayerSync_Message;
				size = sizeof(PlayerSyncPacket) - sizeof(GamePacket);
			}
		};
		struct LobbyPacket : public GamePacket {
			LobbyState	status = LobbyState::Lobby;

			LobbyPacket() {
				type = Lobby_Message;
				size = sizeof(LobbyPacket) - sizeof(GamePacket);
			}
		};
	}
}

