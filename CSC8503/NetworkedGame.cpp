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

#include "GameTechRenderer.h"
#include "Hud.h"

constexpr auto COLLISION_MSG = 30;
constexpr auto OBJECTID_START = 10; //reserve 0-4 for playerID;
constexpr auto BOSSID = 5; //reserve 0-4 for playerID;

constexpr auto MAXCLIENTCONNECT = MAXPLAYER - 1;

NetworkedGame::NetworkedGame(bool isServer)	{
	thisServer = nullptr;
	thisClient = nullptr;

	NetworkBase::Initialise();
	gameStateManager.SetIsNetworked(true);
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
	gameStateManager.SetIsNetworked(false);
	Disconnect();
	delete thisServer;
	delete thisClient;
	localPlayer = nullptr;
	serverPlayers.clear();
}

void NetworkedGame::StartAsServer() {
	SetName("server");
	thisServer = new GameServer(NetworkBase::GetDefaultPort(), MAXCLIENTCONNECT);
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
	thisClient->RegisterPacketHandler(ClientID_Message, this);
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
	if(localPlayer)
		localPlayer->GetCamera()->GetHud().ClearAndErase();
	TutorialGame::Clear();
	for (int i = 0; i < 4; i++) {
		players[0] = nullptr;
	}
	localPlayer = nullptr;
}

void NetworkedGame::StartLobby()
{
	canJoin = true;
	InitWorld();
	SpawnPlayers();
	renderer.SetNumPlayers(1);
	int id = OBJECTID_START;
	BulletInstanceManager::instance().AddNetworkObject(id);

	gameStateManager.SetGameState(GameState::Lobby);
	BroadcastGameStateChange();
}

void NetworkedGame::StartLevel() {
	canJoin = false;

	InitWorld();
	SpawnPlayers();
	int id = OBJECTID_START;
	BulletInstanceManager::instance().AddNetworkObject(id);

	boss = AddNetworkBossToWorld({ 0, 5, -20 }, { 2,2,2 }, 1);
	boss->SetNextTarget(players[0]);
	localPlayer->GetCamera()->GetHud().AddHealthBar(boss->GetHealth(), Vector2(0.0f, -0.8f), Vector2(0.7f, 0.04f));

	gameStateManager.SetGameState(GameState::OnGoing);
	BroadcastGameStateChange();
}

void NetworkedGame::SpawnPlayers() {
	if (thisServer) {
		localPlayer = SpawnPlayer(0, true);
		localPlayer->GetCamera()->GetHud().AddHealthBar(localPlayer->GetHealth(), Vector2(-0.6f, 0.9f), Vector2(0.35f, 0.03f));
		localPlayer->GetCamera()->GetHud().SetPlayerHealth(localPlayer->GetHealth());
	}
	if (thisClient && selfID != NULL) {
		localPlayer = SpawnPlayer(selfID, true);
		localPlayer->GetCamera()->GetHud().AddHealthBar(localPlayer->GetHealth(), Vector2(-0.6f, 0.9f), Vector2(0.35f, 0.03f));
		localPlayer->GetCamera()->GetHud().SetPlayerHealth(localPlayer->GetHealth());
	}
	int count = 1;
	for (int id : connectedPlayerIDs) {
		PlayerObject* p = PlayerJoinedServer(id);
		localPlayer->GetCamera()->GetHud().AddHealthBar(p->GetHealth(), Vector2(-0.7f, 0.9f - 0.1f * count), Vector2(0.25f, 0.02f));
		count++;
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

	TutorialGame::UpdateGame(dt);
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
	if (thisClient) {
		thisClient->Disconnect();
	}
}

void NetworkedGame::BossTarget()
{
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

void NetworkedGame::UpdateAsServer(float dt) {
	Debug::DrawAxisLines(localPlayer->GetTransform().GetGlobalMatrix());
	//update Game state
	float health = 0;
	health += static_cast<NetworkPlayer*>(localPlayer)->GetHealth()->GetHealth();
	for (auto const& player : serverPlayers) {
		health += static_cast<NetworkPlayer*>(player.second)->GetHealth()->GetHealth();
	}
	if (health <= 0) {
		gameStateManager.SetGameState(GameState::Lose);
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
		player->ServerSideMovement(dt);
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
	if (localPlayer) {
		NetworkPlayer* player = static_cast<NetworkPlayer*>(localPlayer);
		if (!player->isFrozen) {
			player->ClientUpdateCamera(dt);
			keyMap.Update();
			newPacket.buttonstates = keyMap.GetButtonState();
			player->GetNetworkAxis(newPacket.axis);
			newPacket.yaw = static_cast<int>(player->GetYaw() * 1000);
			newPacket.pitch = static_cast<int>(player->GetPitch() * 1000);
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
	});
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
	PlayerObject* newPlayer = AddNetworkPlayerToWorld(Vector3(0, 5, 90), playerID);
	if (isSelf) {
		keyMap.ChangePlayerControlTypeMap(selfID, ControllerType::KeyboardMouse);
		SetCameraFollow(newPlayer);
		newPlayer->GetCamera()->SetFollow(&(newPlayer->GetNetworkObject()->GetRenderTransform()));
		players[selfID] = newPlayer;
	}
	serverPlayers[playerID] = newPlayer;
	stateIDs[playerID] = -1;
	return newPlayer;
}

void NetworkedGame::ServerProcessNetworkObject(GamePacket* payload, int playerID) {
	if (!serverPlayers.contains(playerID))
		return;
	
	((NetworkPlayer*)serverPlayers[playerID])->MoveInput(((ClientPacket*)payload)->buttonstates, ((ClientPacket*)payload)->axis, Vector2(((ClientPacket*)payload)->yaw * 0.001f, ((ClientPacket*)payload)->pitch * 0.001f));

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
	if (gameStateManager.GetGameState() != GameState::Lobby) {
		LobbyPacket newPacket;
		newPacket.status = LobbyState::Started;
		thisServer->SendPacket(&newPacket, source, true);
		return;
	}
	if (connectedPlayerIDs.size() > MAXCLIENTCONNECT) {
		LobbyPacket newPacket;
		newPacket.status = LobbyState::Started;
		thisServer->SendPacket(&newPacket, source, true);
		return;
	}

	int playerID = ((PlayerConnectionPacket*)payload)->playerID;
	connectedPlayerIDs.push_back(playerID);
	PlayerObject* newPlayer = NetworkedGame::PlayerJoinedServer(playerID);
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
			if (i == playerID) {
				//send the id to the new player
				ClientIDPacket payload;
				payload.objectID = playerID;
				thisServer->SendPacket(&payload, i, true);
			}
			else {
				//send a new player joined to old player
				PlayerConnectionPacket newPacket;
				newPacket.playerID = playerID;
				thisServer->SendPacket(&newPacket, i);

				//send old player's id to new player
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

void NetworkedGame::HandleClientIDPacket(GamePacket* payload, int source) {
	selfID = ((ClientIDPacket*)payload)->objectID;
	if (thisClient && localPlayer == nullptr) {
		localPlayer = SpawnPlayer(selfID, true);
		localPlayer->GetCamera()->GetHud().AddHealthBar(localPlayer->GetHealth(), Vector2(-0.6f, 0.9f), Vector2(0.35f, 0.03f));
	}
}

void NetworkedGame::HandlePlayerSyncPacket(GamePacket* payload, int source) {
	
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
		gameStateManager.SetGameState(GameState::Quit);
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
	case ClientID_Message:
		HandleClientIDPacket(payload, source);
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

PlayerObject* NetworkedGame::PlayerJoinedServer(int playerID) {
	PlayerObject* temp = SpawnPlayer(playerID);
	return temp;
}

void NetworkedGame::PlayerLeftServer(int playerID) {
	if (serverPlayers.contains(playerID)) {
		if (gameStateManager.GetGameState() == GameState::Lobby) {
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

PlayerObject* NetworkedGame::AddNetworkPlayerToWorld(const Vector3& position, int playerID) {
	static int id = 0;
	NetworkPlayer* character = new NetworkPlayer( this, playerID);
	SphereVolume* volume = new SphereVolume(1.2f, CollisionLayer::Player);

	character->SetBoundingVolume(volume);

	float scale = 2.0f;
	character->GetTransform()
		.SetScale(Vector3(scale))
		.SetPosition(position);

	character->SetRenderObject(new AnimatedRenderObject(character->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("player"), AssetLibrary<MeshMaterial>::GetAsset("player"), AssetLibrary<MeshAnimation>::GetAsset("PlayerIdle")));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetRenderObject()->SetColour(Vector4(1, 0.9f, 0.8f, 1));

	character->GetPhysicsObject()->SetInverseMass(1);
	character->GetPhysicsObject()->InitSphereInertia();
	character->SetNetworkObject(new NetworkObject(*character, playerID));
	gameWorld.AddGameObject(character);

	return character;
}

NetworkBoss* NetworkedGame::AddNetworkBossToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	NetworkBoss* boss = new NetworkBoss(this);

	boss->SetBoundingVolume((CollisionVolume*)new AABBVolume(Vector3{ dimensions.x,dimensions.y * 2.2f,dimensions.z }));

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

	switch (gameStateManager.GetGameState()) {
	case GameState::Win:
	case GameState::Lose:
		if(thisServer){
			Debug::Print("Press [R] or [Start] to return to lobby", Vector2(5, 80), Debug::WHITE);
		}
		if (thisClient) {
			Debug::Print("Please Wait for server to return to lobby", Vector2(5, 80), Debug::WHITE);
		}
		break;
	case GameState::Lobby:
		std::string s = "Connected Player: ";
		s += std::to_string(connectedPlayerIDs.size() + 1 ) + "/" + std::to_string(MAXPLAYER);
		Debug::Print(s.c_str(), Vector2(55, 06), Debug::WHITE);
		if (thisServer) {
			Debug::Print("Press [R] or [Start] to Start the Game", Vector2(5, 80), Debug::WHITE);
		}
		if (thisClient) {
			Debug::Print("Please Wait for server to Start the Game", Vector2(5, 80), Debug::WHITE);
		}
		break;
	}

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