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

#include <bitset>
#include <Maths.h>
#include "PlayerBullet.h"

#define COLLISION_MSG 30
#define OBJECTID_START 10; //reserve 0-4 for playerID

struct MessagePacket : public GamePacket {
	short playerID;
	short messageID;

	MessagePacket() {
		type = Message;
		size = sizeof(short) * 2;
	}
};

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
	if (isServer) {
		std::cout << "start as server" << std::endl;
		StartAsServer();
	}
	else {
		StartAsClient(127, 0, 0, 1);
	}
}

NetworkedGame::~NetworkedGame()	{
	delete thisServer;
	delete thisClient;
	localPlayer = nullptr;
	serverPlayers.clear();
	/*
	for (auto i = serverPlayers.begin(); i != serverPlayers.end(); i++) {
		delete i->second;
	}
	*/
}

void NetworkedGame::StartAsServer() {
	SetName("server");
	thisServer = new GameServer(NetworkBase::GetDefaultPort(), 4);
	thisServer->RegisterPacketHandler(Received_State, this);
	thisServer->RegisterPacketHandler(Player_Connected, this);
	thisServer->RegisterPacketHandler(Player_Disconnected, this);
	thisServer->RegisterPacketHandler(Handshake_Ack, this);

	StartLevel();
}

void NetworkedGame::StartAsClient(char a, char b, char c, char d) {
	SetName("client");
	thisClient = new GameClient();
	thisClient->Connect(a, b, c, d, NetworkBase::GetDefaultPort());
	thisClient->RegisterPacketHandler(Delta_State, this);
	thisClient->RegisterPacketHandler(Full_State, this);
	thisClient->RegisterPacketHandler(Player_Connected, this);
	thisClient->RegisterPacketHandler(Player_Disconnected, this);
	thisClient->RegisterPacketHandler(Handshake_Message, this);
	thisClient->RegisterPacketHandler(Item_Init_Message, this);
	thisClient->RegisterPacketHandler(BossAction_Message, this);

	StartLevel();
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

	world->GetMainCamera()->UpdateCamera(dt);
}

void NetworkedGame::FreezeSelf()
{
	static_cast<NetworkPlayer*>(localPlayer)->isFrozen = true;
}

void NetworkedGame::UnfreezeSelf()
{
	static_cast<NetworkPlayer*>(localPlayer)->isFrozen = false;
}

void NetworkedGame::UpdateAsServer(float dt) {
	thisServer->UpdateServer();
	packetsToSnapshot--;

	//update Game state
	for (auto const& player : serverPlayers) {
		static_cast<NetworkPlayer*>(player.second);
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
	ServerGetInstantiatedObject(player);
	localPlayer->GetNetworkObject()->SnapRenderToSelf();
	std::vector<NetworkObject*> networkObjects = world->GetNetworkObjects();
	bool processed = false;
	for (auto networkObj : networkObjects) {
		networkObj->SnapRenderToSelf();
	}
}

void NetworkedGame::UpdateAsClient(float dt) {
	thisClient->UpdateClient();

	//move obj
	///*
	std::vector<NetworkObject*> networkObjects = world->GetNetworkObjects();
	
	bool processed = false;
	for (auto networkObj: networkObjects) {
		networkObj->UpdateDelta(dt);
	}
	
	//*/
	//send self data to server
	ClientPacket newPacket;
	newPacket.lastID = stateID;
	keyMap.Update();
	newPacket.buttonstates = keyMap.GetButtonState();
	if (!Window::GetKeyboard()->KeyDown(KeyboardKeys::C)) {
		newPacket.yaw = (int) world->GetMainCamera()->GetYaw() * 1000;
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
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;

	world->GetObjectIterators(first, last);

	for (auto i = first; i != last; ++i) {
		NetworkObject* o = (*i)->GetNetworkObject();
		if (!o) {
			continue;
		}

		//TODO - you'll need some way of determining
		//when a player has sent the server an acknowledgement
		//and store the lastID somewhere. A map between player
		//and an int could work, or it could be part of a 
		//NetworkPlayer struct. 

		int playerState = stateID;
		GamePacket* newPacket = nullptr;
		if (o->WritePacket(&newPacket, deltaFrame, playerState)) {
			thisServer->SendGlobalPacket(newPacket);
			delete newPacket;
		}
	}
}

void NetworkedGame::SendSnapshot(bool deltaFrame, int playerID) {
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;

	world->GetObjectIterators(first, last);

	for (auto i = first; i != last; ++i) {
		NetworkObject* o = (*i)->GetNetworkObject();
		if (!o) {
			continue;
		}
		int playerState = stateIDs[playerID];

		GamePacket* newPacket = nullptr;
		if (o->WritePacket(&newPacket, deltaFrame, playerState)) {
			thisServer->SendPacket(newPacket, playerID);
			delete newPacket;
		}
	}
	BossActionPacket newPacket;
	newPacket.bossAction = static_cast<short int> (testingBossBehaviorTree->GetBossAction());
	thisServer->SendPacket(static_cast <GamePacket*> (&newPacket), playerID);
}

void NetworkedGame::UpdateMinimumState() {
	//Periodically remove old data from the server
	int minID = INT_MAX;
	int maxID = 0; //we could use this to see if a player is lagging behind?

	for (auto i : stateIDs) {
		minID = std::min(minID, i.second);
		maxID = std::max(maxID, i.second);
	}
	//every client has acknowledged reaching at least state minID
	//so we can get rid of any old states!
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;
	world->GetObjectIterators(first, last);

	for (auto i = first; i != last; ++i) {
		NetworkObject* o = (*i)->GetNetworkObject();
		if (!o) {
			continue;
		}
		o->UpdateStateHistory(minID); //clear out old states so they arent taking up memory...
	}
}

GameObject* NetworkedGame::SpawnPlayer(int playerID, bool isSelf){
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
	GameObject* newPlayer = AddNetworkPlayerToWorld(Vector3(0, 5, 90), isSelf, playerID);
	if (isSelf) {
		world->GetMainCamera()->SetFollow(&(newPlayer->GetNetworkObject()->GetRenderTransform()));
	}
	serverPlayers[playerID] = newPlayer;
	stateIDs[playerID] = -1;
	return newPlayer;
}

void NetworkedGame::StartLevel() {
	InitWorld();
	testingBoss = AddBossToWorld({ 0, 5, -20 }, { 2,2,2 }, 1);
	testingBossBehaviorTree = new BossBehaviorTree(testingBoss);
	testingBoss->SetNetworkObject(new NetworkObject(*testingBoss, 10));
	world->AddNetworkObject(testingBoss->GetNetworkObject());
	if (thisServer) {
		localPlayer = SpawnPlayer(0, true);
		player = static_cast<PlayerObject*>(localPlayer);
		testingBossBehaviorTree->ChangeTarget(static_cast<PlayerObject*>(localPlayer));
	}
}

void NetworkedGame::ServerProcessNetworkObject(GamePacket* payload, int playerID) {
	//rotation
	((NetworkPlayer*)serverPlayers[playerID])->MoveInput(((ClientPacket*)payload)->buttonstates);

	if (((ClientPacket*)payload)->yaw != NULL) {
		((NetworkPlayer*)serverPlayers[playerID])->RotateYaw(((ClientPacket*)payload)->yaw*0.001);
	}

	if (((ClientPacket*)payload)->lastID > stateIDs[playerID]) {
		stateIDs[playerID] = ((ClientPacket*)payload)->lastID;
	}
	
	ServerGetInstantiatedObject((NetworkPlayer*)serverPlayers[playerID]);
}

void NetworkedGame::ClientProcessNetworkObject(GamePacket* payload, int objID) {
	std::vector<NetworkObject*> networkObjects = world->GetNetworkObjects();

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

void NetworkedGame::ServerGetInstantiatedObject(NetworkPlayer* player) {
	vector<GameObject*> newObjList = player->GetLastInstancedObjects();
	for (auto newObj : newObjList) {
		if (newObj->GetNetworkObject() == nullptr) {
			newObj->SetNetworkObject(new NetworkObject(*newObj, newObj->GetWorldID()));
			world->AddNetworkObject(newObj->GetNetworkObject());
			SendInitItemPacket(newObj);
		}

	}
}

void NetworkedGame::SendInitItemPacket(GameObject* obj) {
	ItemInitPacket newObj; 
	Transform objTransform = obj->GetTransform();
	newObj.position = objTransform.GetGlobalPosition();
	newObj.orientation = objTransform.GetGlobalOrientation();
	newObj.scale = objTransform.GetScale();
	newObj.velocity = obj->GetPhysicsObject()->GetLinearVelocity();
	newObj.objectID = obj->GetNetworkObject()->GetNetworkID();

	newObj.itemType = NetworkInstanceType::Projectile;

	thisServer->SendGlobalPacket(&newObj);
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

	int playerID = ((PlayerConnectionPacket*)payload)->playerID;
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
		std::vector<int> connectedClients = thisServer->GetClientIDs();
		for (auto i : connectedClients) {
			//std::cout << "Server sending to : " << i << std::endl;
			if (i == playerID) {
				//send the id to the new player
				//std::cout << playerID << " : " << i << std::endl;
				HandshakePacket payload;
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

void NetworkedGame::HandleHandshakePacket(GamePacket* payload, int source) {
	std::cout << "Handshake_Message" << std::endl;
	selfID = ((HandshakePacket*)payload)->objectID;
	std::cout << name << " " << selfID << " recieved Handshake_Message" << std::endl;
	if (thisClient && localPlayer == nullptr) {
		std::cout << name << " " << selfID << " Creating localhost player " << selfID << std::endl;
		localPlayer = SpawnPlayer(selfID, true);
	}
}

void NetworkedGame::HandleItemInitPacket(GamePacket* payload, int source) {
	GameObject* obj;
	switch ( ((int)((ItemInitPacket*)payload)->itemType) )
	{
		//server
	case NetworkInstanceType::Projectile:
		obj = new PlayerBullet(*(PlayerBullet*)PrefabLibrary::GetPrefab("bullet"));
		((PlayerBullet*)obj)->SetLifespan(5);
		break;
	default:
		obj = new PlayerBullet(*(PlayerBullet*)PrefabLibrary::GetPrefab("bullet"));
		break;
	}
	obj->GetTransform().SetPosition(((ItemInitPacket*)payload)->position);
	obj->GetPhysicsObject()->SetInverseMass(2.0f);
	obj->GetPhysicsObject()->ApplyLinearImpulse(((ItemInitPacket*)payload)->velocity);
	obj->SetNetworkObject(new NetworkObject(*obj, ((int)((ItemInitPacket*)payload)->objectID)));
	obj->GetNetworkObject()->SnapRenderToSelf();
	world->AddGameObject(obj);
}

void NetworkedGame::HandleBossActionPacket(GamePacket* payload, int source)
{
	std::cout << "receiving boss action" << std::endl;
	BossBehaviorTree::BossAction action = static_cast<BossBehaviorTree::BossAction>(static_cast<BossActionPacket*>(payload)->bossAction);
	testingBossBehaviorTree->SetBossAction(action);
}

void NetworkedGame::ReceivePacket(int type, GamePacket* payload, int source) {

	switch (type)
	{
		//server
	case Received_State:
		ServerProcessNetworkObject(payload, source);
		break;
	case Handshake_Ack:
		thisServer->handshakeMap[source] = true;
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
	case Handshake_Message:
		HandleHandshakePacket(payload, source);
		break;
	case Item_Init_Message:
		HandleItemInitPacket(payload, source);
		break;
	case BossAction_Message:
		HandleBossActionPacket(payload, source);
		break;
	default:
		break;
	}
}

void NetworkedGame::PlayerJoinedServer(int playerID) {
	PlayerObject* temp = static_cast<PlayerObject*>(SpawnPlayer(playerID));
	if (testingBossBehaviorTree) {
		testingBossBehaviorTree->ChangeTarget(static_cast<PlayerObject*>(temp));
	}

}

void NetworkedGame::PlayerLeftServer(int playerID) {
	//std::cout << name << " " << selfID << " deleting player " << playerID << std::endl;
	if (serverPlayers[playerID] != nullptr) {
		world->RemoveGameObject(serverPlayers[playerID], false);
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

	character->SetRenderObject(new RenderObject(&character->GetTransform(), charMesh, nullptr));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetRenderObject()->SetColour(Vector4(1, 0.9f, 0.8f, 1));

	character->GetPhysicsObject()->SetInverseMass(1);
	character->GetPhysicsObject()->InitSphereInertia();
	character->SetNetworkObject(new NetworkObject(*character, playerID));
	world->AddGameObject(character);

	if (cameraFollow) {
		world->GetMainCamera()->SetFollow(&character->GetTransform());
		character->AttachedCamera();
	}

	return character;
}
