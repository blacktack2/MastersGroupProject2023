/**
 * @file   NetworkGame.h
 * @brief  See NetworkGame.cpp
 *
 * @author Rich Davidson
 * @author Felix Chiu
 * @date   Dec 2022
 */
#include "NetworkedGame.h"
#include "NetworkPlayer.h"
#include "NetworkObject.h"
#include "RenderObject.h"
#include "GameServer.h"
#include "GameClient.h"

#include "PlayerObject.h"
#include "TutorialGame.h"

#include "Bullet.h"
#include "AssetLibrary.h"
#include "PrefabLibrary.h"
#include "BulletInstanceManager.h"

#include <bitset>
#include <Maths.h>
#include "PlayerBullet.h"

constexpr auto COLLISION_MSG = 30;
constexpr auto OBJECTID_START = 10; //reserve 0-4 for playerID;
constexpr auto BOSSID = 5; //reserve 0-4 for playerID;

NetworkedGame::NetworkedGame(bool isServer)	{
	thisServer = nullptr;
	thisClient = nullptr;

	NetworkBase::Initialise();
	timeToNextPacket  = 0.0f;
	packetsToSnapshot = 0;
	stateID = 0;
	selfID = 0;
	game_dt = 0;
	objectID = OBJECTID_START;
	gameStateManager.SetGameState(GameState::Lobby);
	if (isServer) {
		std::cout << "start as server" << std::endl;
		StartAsServer();
	}
	else {
		StartAsClient(127, 0, 0, 1);
	}
}

NetworkedGame::~NetworkedGame()	{
	Disconnect();
	delete thisServer;
	delete thisClient;
	localPlayer = nullptr;
	serverPlayers.clear();
}

void NetworkedGame::StartAsServer() {
	SetName("server");
	thisServer = new GameServer(NetworkBase::GetDefaultPort(), 4);
	thisServer->RegisterPacketHandler(Received_State, this);
	thisServer->RegisterPacketHandler(Player_Connected, this);
	thisServer->RegisterPacketHandler(Player_Disconnected, this);

	StartLobby();
}

void NetworkedGame::StartAsClient(char a, char b, char c, char d) {
	SetName("client");
	thisClient = new GameClient();
	thisClient->Connect(a, b, c, d, NetworkBase::GetDefaultPort());
	thisClient->RegisterPacketHandler(Delta_State, this);
	thisClient->RegisterPacketHandler(Full_State, this);
	thisClient->RegisterPacketHandler(Player_Connected, this);
	thisClient->RegisterPacketHandler(Player_Disconnected, this);
	thisClient->RegisterPacketHandler(PlayerSync_Message, this);
	thisClient->RegisterPacketHandler(Item_Init_Message, this);
	thisClient->RegisterPacketHandler(BossAction_Message, this);
	thisClient->RegisterPacketHandler(GameState_Message, this);
	thisClient->RegisterPacketHandler(Lobby_Message, this);

	ClientPacket newPacket;
	newPacket.lastID = stateID;

	thisClient->SendPacket(&newPacket);

	StartLobby();
}


void NetworkedGame::Clear()
{
	TutorialGame::Clear();
	player = NULL;
	localPlayer = NULL;
}

void NetworkedGame::StartLobby()
{
	canJoin = true;

	InitWorld();
	SpawnPlayers();
	int id = OBJECTID_START;
	BulletInstanceManager::instance().AddNetworkObject(id);

	gameStateManager->SetGameState(GameState::Lobby);
	BroadcastGameStateChange();
}

void NetworkedGame::StartLevel() {
	canJoin = false;

	InitWorld();
	SpawnPlayers();
	int id = OBJECTID_START;
	BulletInstanceManager::instance().AddNetworkObject(id);

	boss = AddNetworkBossToWorld({ 0, 5, -20 }, { 2,2,2 }, 1);
	boss->SetNextTarget(player);

	gameStateManager->SetGameState(GameState::OnGoing);
	BroadcastGameStateChange();
}

void NetworkedGame::SpawnPlayers() {
	if (thisServer) {
		localPlayer = SpawnPlayer(0, true);
	}
	if (thisClient && selfID != NULL) {
		localPlayer = SpawnPlayer(selfID, true);
	}
	for (int id : connectedPlayerIDs) {
		PlayerJoinedServer(id);
	}
}

void NetworkedGame::BroadcastGameStateChange()
{
	if (thisServer) {
		GameStatePacket packet;
		packet.state = gameStateManager.GetGameState();
		thisServer->SendGlobalPacket(static_cast<GamePacket*>(&packet));
	}
}

void NetworkedGame::UpdateGame(float dt) {
	game_dt = dt;
	timeToNextPacket -= dt;
	if (timeToNextPacket < 0) {
		if (thisServer) {
			UpdateAsServer(dt);
		}
		else if (thisClient) {
			UpdateAsClient(dt);
		}
		timeToNextPacket = packetGap;
	}

	//boss special
	if (boss) {
		//Change boss target
		Vector3 displacement;
		PlayerObject* target = boss->GetTarget();
		PlayerObject* player;
		float minDist = 300;
		for (auto pair : serverPlayers) {
			player = static_cast<PlayerObject*>(pair.second);
			if (player->GetHealth()->GetHealth() <= 0) {
				continue;
			}
			displacement = player->GetTransform().GetGlobalPosition() - boss->GetTransform().GetGlobalPosition();
			float dist = abs(displacement.Length());
			if (dist < minDist) {
				target = static_cast<PlayerObject*>(player);
				minDist = dist;
			}
		}
		boss->SetNextTarget(target);
	}

	TutorialGame::UpdateGame(dt);

	gameWorld.GetMainCamera()->UpdateCamera(dt);
}

void NetworkedGame::FreezeSelf()
{
	if(localPlayer)
		static_cast<NetworkPlayer*>(localPlayer)->isFrozen = true;
}

void NetworkedGame::UnfreezeSelf()
{
	if (localPlayer)
		static_cast<NetworkPlayer*>(localPlayer)->isFrozen = false;
}

GameServer* NetworkedGame::GetServer()
{
	return thisServer;
}

void NetworkedGame::Disconnect()
{
	std::cout << "attempt disconnect" << std::endl;
	if (thisClient) {
		thisClient->Disconnect();
	}
}

void NetworkedGame::UpdateAsServer(float dt) {
	//update Game state
	float health = 0;
	health += static_cast<NetworkPlayer*>(localPlayer)->GetHealth()->GetHealth();
	for (auto const& player : serverPlayers) {
		health += static_cast<NetworkPlayer*>(player.second)->GetHealth()->GetHealth();
	}
	if (health <= 0) {
		gameStateManager->SetGameState(GameState::Lose);
		BroadcastGameStateChange();
	}

	//send important information to each player
	std::vector<int> connectedClients = thisServer->GetClientIDs();
	///*
	if (packetsToSnapshot < 0) {
		for (auto i : connectedClients) {
			SendSnapshot(false, i);
		}
		packetsToSnapshot = fullPacketToDeltaRate;
	}
	else {
		for (auto i : connectedClients) {
			SendSnapshot(true, i);
		}
	}

	//move main player
	NetworkPlayer* player = static_cast<NetworkPlayer*>(localPlayer);
	if (!player->isFrozen) {
		player->ServerSideMovement();
	}
	localPlayer->GetNetworkObject()->SnapRenderToSelf();
	std::vector<NetworkObject*> networkObjects = gameWorld.GetNetworkObjects();
	bool processed = false;
	for (auto networkObj : networkObjects) {
		networkObj->SnapRenderToSelf();
	}

	thisServer->UpdateServer();
	packetsToSnapshot--;

	if (gameStateManager.GetGameState() == GameState::Lobby) {
		if (keyMap.GetButton(Start)) {
			std::cout << "Start Level" << std::endl;
			StartLevel();
		}
	}
}

void NetworkedGame::UpdateAsClient(float dt) {
	thisClient->UpdateClient();

	//move obj
	///*
	std::vector<NetworkObject*> networkObjects = gameWorld.GetNetworkObjects();
	
	bool processed = false;
	for (auto networkObj: networkObjects) {
		if(networkObj->isActive())
			networkObj->UpdateDelta(dt);
	}
	
	//*/
	//send self data to server
	ClientPacket newPacket;
	newPacket.lastID = stateID;
	keyMap.Update();
	newPacket.buttonstates = keyMap.GetButtonState();
	if (!Window::GetKeyboard()->KeyDown(KeyboardKeys::C)) {
		newPacket.yaw = (int) gameWorld.GetMainCamera()->GetYaw() * 1000;
	}
	else {
		newPacket.yaw = NULL;
	}
	if (localPlayer) {
		NetworkPlayer* player = static_cast<NetworkPlayer*>(localPlayer);
		if (player->isFrozen) {
			newPacket.buttonstates = 0;
			newPacket.yaw = NULL;
		}
	}
	
	thisClient->SendPacket(&newPacket);
}

void NetworkedGame::BroadcastSnapshot(bool deltaFrame) {
	gameWorld.OperateOnContents([&](GameObject* object) {
		NetworkObject* networkObj = object->GetNetworkObject();
		if (!networkObj) {
			return;
		}

		//TODO - you'll need some way of determining
		//when a player has sent the server an acknowledgement
		//and store the lastID somewhere. A map between player
		//and an int could work, or it could be part of a 
		//NetworkPlayer struct. 

		int playerState = stateID;
		GamePacket* newPacket = nullptr;
		if (networkObj->WritePacket(&newPacket, deltaFrame, playerState)) {
			thisServer->SendGlobalPacket(newPacket);
			delete newPacket;
		}
	});
}

void NetworkedGame::SendSnapshot(bool deltaFrame, int playerID) {
	gameWorld.OperateOnContents([&](GameObject* object) {
		if (!object->IsActive()) {
			return;
		}
		NetworkObject* networkObj = object->GetNetworkObject();
		if (!networkObj) {
			return;
		}
		int playerState = stateIDs[playerID];

		GamePacket* newPacket = nullptr;
		if (networkObj->WritePacket(&newPacket, deltaFrame, playerState)) {
			thisServer->SendPacket(newPacket, playerID);
			delete newPacket;
		}
	}
	if (boss) {
		BossActionPacket newPacket;
		newPacket.bossAction = static_cast<short int> (boss->GetBossAction());
		thisServer->SendPacket(static_cast <GamePacket*> (&newPacket), playerID);
	}
}

void NetworkedGame::UpdateMinimumState() {
	//Periodically remove old data from the server
	int minID = INT_MAX;
	int maxID = 0; //we could use this to see if a player is lagging behind?

	for (auto& idPair : stateIDs) {
		minID = std::min(minID, idPair.second);
		maxID = std::max(maxID, idPair.second);
	}
	//every client has acknowledged reaching at least state minID
	//so we can get rid of any old states!
	gameWorld.OperateOnContents([&](GameObject* object) {
		NetworkObject* networkObj = object->GetNetworkObject();
	if (!networkObj) {
		return;
	}
	networkObj->UpdateStateHistory(minID); //clear out old states so they arent taking up memory...
		});
}

PlayerObject* NetworkedGame::SpawnPlayer(int playerID, bool isSelf){
	Vector4 colour;
	colour = Vector4(1, 0, 1, 1);
	if (playerID == 0) {
		colour = Vector4(1, 0, 0, 1);
	}
	else if (playerID == 1) {
		colour = Vector4(0, 1, 0, 1);
	}
	else if (playerID == 2) {
		colour = Vector4(0, 0, 1, 1);
	}
	else if (playerID == 3) {
		colour = Vector4(0, 1, 1, 1);
	}
	PlayerObject* newPlayer = AddNetworkPlayerToWorld(Vector3(0, 5, 90), isSelf, playerID);
	if (isSelf) {
		gameWorld.GetMainCamera()->SetFollow(&(newPlayer->GetNetworkObject()->GetRenderTransform()));
		player = newPlayer;
	}
	serverPlayers[playerID] = newPlayer;
	stateIDs[playerID] = -1;
	return newPlayer;
}

void NetworkedGame::ServerProcessNetworkObject(GamePacket* payload, int playerID) {
	if (!serverPlayers.contains(playerID))
		return;
	//rotation
	((NetworkPlayer*)serverPlayers[playerID])->MoveInput(((ClientPacket*)payload)->buttonstates);

	if (((ClientPacket*)payload)->yaw != NULL) {
		((NetworkPlayer*)serverPlayers[playerID])->RotateYaw(((ClientPacket*)payload)->yaw * 0.001f);
	}

	if (((ClientPacket*)payload)->lastID > stateIDs[playerID]) {
		stateIDs[playerID] = ((ClientPacket*)payload)->lastID;
	}

}

void NetworkedGame::ClientProcessNetworkObject(GamePacket* payload, int objID) {
	std::vector<NetworkObject*> networkObjects = gameWorld.GetNetworkObjects();

	bool processed = false;
	for (auto networkObj : networkObjects) {
		if (networkObj->ReadPacket(*payload, game_dt)) {
			processed = true;
		}
	}
	if (!processed) {
		//spawn?
	}
}

void NetworkedGame::HandleDeltaPacket(GamePacket* payload, int source) {
	ClientProcessNetworkObject(payload, ((DeltaPacket*)payload)->objectID);
}

void NetworkedGame::HandleFullPacket(GamePacket* payload, int source){
	ClientProcessNetworkObject(payload, ((FullPacket*)payload)->objectID);
	if (((FullPacket*)payload)->fullState.stateID > stateID) {
		stateID = ((FullPacket*)payload)->fullState.stateID;
	}
}

void NetworkedGame::HandlePlayerConnectedPacket(GamePacket* payload, int source) {
	if (gameStateManager->GetGameState() != GameState::Lobby) {
		LobbyPacket newPacket;
		newPacket.status = LobbyState::Started;
		thisServer->SendPacket(&newPacket, source, true);
		return;
	}

	int playerID = ((PlayerConnectionPacket*)payload)->playerID;
	connectedPlayerIDs.push_back(playerID);
	NetworkedGame::PlayerJoinedServer(playerID);

	//if server: send to clients
	if (thisServer) {
		//send server object;
		if (localPlayer) {
			//send server player for new player
			PlayerConnectionPacket existingPacket;
			existingPacket.playerID = 0;
			thisServer->SendPacket(&existingPacket, playerID, true);
		}
		std::vector<int> connectedClients = connectedPlayerIDs;
		for (auto i : connectedClients) {
			//std::cout << "Server sending to : " << i << std::endl;
			if (i == playerID) {
				//send the id to the new player
				//std::cout << playerID << " : " << i << std::endl;
				PlayerSyncPacket payload;
				payload.objectID = playerID;
				thisServer->SendPacket(&payload, i, true);
			}
			else {
				//send a new player joined to old player
				PlayerConnectionPacket newPacket;
				newPacket.playerID = playerID;
				thisServer->SendPacket(&newPacket, i);

				//send old player's id to new player
				//std::cout << "Server sending create player " << i << " to : " << playerID<< std::endl;
				PlayerConnectionPacket existingPacket;
				existingPacket.playerID = i;
				thisServer->SendPacket(&existingPacket, playerID, true);
			}
		}
	}
}

void NetworkedGame::HandlePlayerDisconnectedPacket(GamePacket* payload, int source) {
	//player disconnected
	int playerID = ((PlayerConnectionPacket*)payload)->playerID;
	NetworkedGame::PlayerLeftServer(playerID);
	//server tell everyone to delete this
	if (thisServer) {
		PlayerConnectionPacket newPacket = PlayerConnectionPacket();
		newPacket.type = Player_Disconnected;
		newPacket.playerID = playerID;
		thisServer->SendGlobalPacket(&newPacket, true);
	}
}

void NetworkedGame::HandlePlayerSyncPacket(GamePacket* payload, int source) {
	std::cout << "PlayerSyncPacket" << std::endl;
	selfID = ((PlayerSyncPacket*)payload)->objectID;
	std::cout << name << " " << selfID << " recieved PlayerSyncPacket" << std::endl;
	if (thisClient && localPlayer == nullptr) {
		std::cout << name << " " << selfID << " Creating localhost player " << selfID << std::endl;
		localPlayer = SpawnPlayer(selfID, true);
		player = static_cast<PlayerObject*> (localPlayer);
	}
}

void NetworkedGame::HandleItemInitPacket(GamePacket* payload, int source) {

}

void NetworkedGame::HandleBossActionPacket(GamePacket* payload, int source)
{
	//Needs Implementing in Boss's Code
	
	Boss::BossAction action = static_cast<Boss::BossAction>(static_cast<BossActionPacket*>(payload)->bossAction);
	if (boss) {
		//boss->SetBossAction(action);
	}
}

void NetworkedGame::HandleGameStatePacket(GamePacket* payload, int source) {
	GameState currentState = gameStateManager.GetGameState();
	GameState newState = static_cast<GameState>(static_cast<GameStatePacket*>(payload)->state);
	if (currentState == GameState::Lobby && newState == GameState::OnGoing) {
		StartLevel();
	}
	if (currentState == GameState::Lose && newState == GameState::Lobby) {
		StartLobby();
	}
	if (currentState == GameState::Win && newState == GameState::Lobby) {
		StartLobby();
	}
	
	gameStateManager.SetGameState(static_cast<GameState>(static_cast<GameStatePacket*>(payload)->state));
}

void NetworkedGame::HandleLobbyPacket(GamePacket* payload, int source)
{
	LobbyState lobbyState = static_cast<LobbyPacket*>(payload)->status;

	switch (lobbyState)
	{
	case LobbyState::Lobby:
		break;
	case LobbyState::Full:
	case LobbyState::Started:
		std::cout << "Quit due to lobby" << std::endl;
		gameStateManager->SetGameState(GameState::Quit);
		break;
	default:
		break;
	}
}

void NetworkedGame::ReceivePacket(int type, GamePacket* payload, int source) {

	switch (type)
	{
		//server
	case Received_State:
		ServerProcessNetworkObject(payload, source);
		break;
		//clients
	case Delta_State:
		HandleDeltaPacket(payload, source);
		break;
	case Full_State:
		HandleFullPacket(payload, source);
		break;
	case Player_Connected:
		HandlePlayerConnectedPacket(payload, source);
		break;
	case Player_Disconnected:
		HandlePlayerDisconnectedPacket(payload, source);
		break;
	case PlayerSync_Message:
		HandlePlayerSyncPacket(payload, source);
		break;
	case Item_Init_Message:
		ClientProcessNetworkObject(payload, source);
		break;
	case BossAction_Message:
		HandleBossActionPacket(payload, source);
		break;
	case GameState_Message:
		HandleGameStatePacket(payload, source);
		break;
	case Lobby_Message:
		HandleLobbyPacket(payload, source);
		break;
	default:
		break;
	}
}

void NetworkedGame::PlayerJoinedServer(int playerID) {
	PlayerObject* temp = static_cast<PlayerObject*>(SpawnPlayer(playerID));
}

void NetworkedGame::PlayerLeftServer(int playerID) {
	if (serverPlayers.contains(playerID)) {
		std::cout << name << " " << selfID << " deleting player " << playerID << std::endl;
		if (gameStateManager->GetGameState() == GameState::Lobby) {
			serverPlayers[playerID]->SetActive(false);
		}
		serverPlayers.erase(playerID);
		connectedPlayerIDs.erase(remove(connectedPlayerIDs.begin(), connectedPlayerIDs.end(), playerID));
	}
		
}

void NetworkedGame::OnPlayerCollision(NetworkPlayer* a, NetworkPlayer* b) {
	/*
	if (thisServer) { //detected a collision between players!
		MessagePacket newPacket;
		newPacket.messageID = COLLISION_MSG;
		newPacket.playerID  = a->GetPlayerNum();

		thisClient->SendPacket(&newPacket);

		newPacket.playerID = b->GetPlayerNum();
		thisClient->SendPacket(&newPacket);
	}
	*/
}

PlayerObject* NetworkedGame::AddNetworkPlayerToWorld(const Vector3& position, bool cameraFollow, int playerID) {
	NetworkPlayer* character = new NetworkPlayer( this, playerID);
	SphereVolume* volume = new SphereVolume(1.0f, CollisionLayer::Player);

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(1, 1, 1))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(character->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("goat"), nullptr));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetRenderObject()->SetColour(Vector4(1, 0.9f, 0.8f, 1));

	character->GetPhysicsObject()->SetInverseMass(1);
	character->GetPhysicsObject()->InitSphereInertia();
	character->SetNetworkObject(new NetworkObject(*character, playerID));
	gameWorld.AddGameObject(character);

	if (cameraFollow) {
		gameWorld.GetMainCamera()->SetFollow(&character->GetTransform());
		character->AttachedCamera();
	}

	return character;
}

NetworkBoss* NetworkedGame::AddNetworkBossToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	NetworkBoss* boss = new NetworkBoss(this);

	boss->SetBoundingVolume((CollisionVolume*)new AABBVolume(dimensions));

	boss->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	boss->SetRenderObject(new AnimatedRenderObject(boss->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("boss"), AssetLibrary<MeshMaterial>::GetAsset("boss"), AssetLibrary<MeshAnimation>::GetAsset("WalkForward")));

	boss->GetRenderObject()->SetColour({ 1,1,1,1 });
	boss->SetPhysicsObject(new PhysicsObject(&boss->GetTransform(), boss->GetBoundingVolume()));

	boss->GetPhysicsObject()->SetInverseMass(inverseMass);
	boss->GetPhysicsObject()->InitCubeInertia();

	int id = BOSSID;
	boss->SetNetworkObject(new NetworkObject(*boss, id));
	gameWorld.AddNetworkObject(boss->GetNetworkObject());

	gameWorld.AddGameObject(boss);

	return boss;
}

void NetworkedGame::ProcessState() {
	NCL::InputKeyMap& keyMap = NCL::InputKeyMap::instance();
	if (thisServer) {
		if (keyMap.GetButton(InputType::Start)) {
			std::cout << "Starting" << std::endl;
			this->StartLevel();
		}
		if (keyMap.GetButton(InputType::Restart)) {
			std::cout << "Return to lobby" << std::endl;
			this->StartLobby();
		}
	}
	
	if (keyMap.GetButton(InputType::Return)) {
		gameStateManager.SetGameState(GameState::Quit);
	}
}