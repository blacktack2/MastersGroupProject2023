#include "NetworkedGame.h"
#include "NetworkPlayer.h"
#include "NetworkObject.h"
#include "RenderObject.h"
#include "GameServer.h"
#include "GameClient.h"

#include "PlayerObject.h"
#include "TutorialGame.h"

#include <bitset>

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

NetworkedGame::NetworkedGame()	{
	thisServer = nullptr;
	thisClient = nullptr;

	NetworkBase::Initialise();
	timeToNextPacket  = 0.0f;
	packetsToSnapshot = 0;
	stateID = 0;
	selfID = 0;
}

NetworkedGame::~NetworkedGame()	{
	delete thisServer;
	delete thisClient;
	for (auto i = serverPlayers.begin(); i != serverPlayers.end(); i++) {
		delete i->second;
	}
}

void NetworkedGame::StartAsServer() {
	thisServer = new GameServer(NetworkBase::GetDefaultPort(), 4);
	thisServer->RegisterPacketHandler(Received_State, this);
	thisServer->RegisterPacketHandler(Player_Connected, this);
	thisServer->RegisterPacketHandler(Player_Disconnected, this);
	thisServer->RegisterPacketHandler(Handshake_Ack, this);

	StartLevel();
	
	localPlayer = SpawnPlayer(0, true);
}

void NetworkedGame::StartAsClient(char a, char b, char c, char d) {
	thisClient = new GameClient();
	thisClient->Connect(a, b, c, d, NetworkBase::GetDefaultPort());
	thisClient->RegisterPacketHandler(Delta_State, this);
	thisClient->RegisterPacketHandler(Full_State, this);
	thisClient->RegisterPacketHandler(Player_Connected, this);
	thisClient->RegisterPacketHandler(Player_Disconnected, this);
	thisClient->RegisterPacketHandler(Handshake_Message, this);

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
		timeToNextPacket += 1.0f / 20.0f; //20hz server/client update
	}

	if (!thisServer && Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9)) {
		SetName("server");
		StartAsServer();
	}
	if (!thisClient && Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10)) {
		SetName("client");
		StartAsClient(127,0,0,1);
		
	}
	TutorialGame::UpdateGame(dt);
}

void NetworkedGame::UpdateAsServer(float dt) {
	thisServer->UpdateServer();
	packetsToSnapshot--;

	//send important information to each player
	std::vector<int> connectedClients = thisServer->GetClientIDs();
	///*
	if (packetsToSnapshot < 0) {
		for (auto i : connectedClients) {
			SendSnapshot(false, i);
		}
		packetsToSnapshot = 10;
	}
	else {
		for (auto i : connectedClients) {
			SendSnapshot(true, i);
		}
	}

	//move main player
	((NetworkPlayer*)localPlayer)->Update(dt);
	((NetworkPlayer*)localPlayer)->Test();

}

void NetworkedGame::UpdateAsClient(float dt) {
	thisClient->UpdateClient();
	ClientPacket newPacket;
	newPacket.lastID = stateID;
	keyMap.Update();
	newPacket.buttonstates = keyMap.GetButtonState();
	//std::cout << "client: " << std::bitset<16>(newPacket.buttonstates) << std::endl;
	//std::cout << "client yaw: " << world->GetMainCamera()->GetYaw() << std::endl;
	if (!Window::GetKeyboard()->KeyDown(KeyboardKeys::C)) {
		newPacket.yaw = world->GetMainCamera()->GetYaw() * 1000;
	}
	else {
		newPacket.yaw = NULL;
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
}

void NetworkedGame::UpdateMinimumState() {
	//Periodically remove old data from the server
	int minID = INT_MAX;
	int maxID = 0; //we could use this to see if a player is lagging behind?

	for (auto i : stateIDs) {
		minID = min(minID, i.second);
		maxID = max(maxID, i.second);
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
	GameObject* newPlayer = AddNetworkPlayerToWorld(Vector3(5, 5, 5), isSelf, playerID);
	networkObjects.push_back(new NetworkObject(*newPlayer, playerID));
	serverPlayers[playerID] = newPlayer;
	stateIDs[playerID] = -1;
	return newPlayer;
}

void NetworkedGame::StartLevel() {
	world->Clear();
	physics->Clear();
	int id = OBJECTID_START;
	int idOffset = 0;
	InitGameExamples();
	InitDefaultFloor();
	world->UpdateStaticTree();
}

void NetworkedGame::ServerProcessNetworkObject(GamePacket* payload, int playerID) {
	//rotation
	//std::cout << "Server: " << std::bitset<16>(((ClientPacket*)payload)->buttonstates) << std::endl;
	//std::cout << "Server yaw : " << ((ClientPacket*)payload)->yaw * 0.001 << std::endl;
	((NetworkPlayer*)serverPlayers[playerID])->MoveInput(((ClientPacket*)payload)->buttonstates);

	if (((ClientPacket*)payload)->yaw != NULL) {
		((NetworkPlayer*)serverPlayers[playerID])->RotateYaw(((ClientPacket*)payload)->yaw*0.001);
	}

	if (((ClientPacket*)payload)->lastID > stateIDs[playerID]) {
		stateIDs[playerID] = ((ClientPacket*)payload)->lastID;
	}
	
}

void NetworkedGame::ClientProcessNetworkObject(GamePacket* payload, int objID) {
	std::vector<NetworkObject*>::const_iterator first;
	std::vector<NetworkObject*>::const_iterator last;
	first = networkObjects.begin();
	last = networkObjects.end();
	bool processed = false;
	for (auto i = first; i != last; ++i) {
		if ((*i)->ReadPacket(*payload, game_dt)) {
			processed = true;
		}
	}
	if (!processed) {
		//spawn?
	}
}

void NetworkedGame::ReceivePacket(int type, GamePacket* payload, int source) {
	//server
	if (type == Received_State) {
		ServerProcessNetworkObject(payload, source);
	}
	if (type == Handshake_Ack) {
		thisServer->handshakeMap[source] = true;
	}
	//clients
	if (type == Delta_State) {
		ClientProcessNetworkObject(payload, ((DeltaPacket*)payload)->objectID);
	}
	if (type == Full_State) {
		ClientProcessNetworkObject(payload, ((FullPacket*)payload)->objectID);
		if (((FullPacket*)payload)->fullState.stateID > stateID) {
			stateID = ((FullPacket*)payload)->fullState.stateID;
		}
			
	}
	if (type == Player_Connected) {

		int playerID = ((PlayerConnectionPacket*)payload)->playerID;
		NetworkedGame::PlayerJoined(playerID);
			
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
	if (type == Player_Disconnected) {
		//player disconnected
		int playerID = ((PlayerConnectionPacket*)payload)->playerID;
		NetworkedGame::PlayerLeft(playerID);
		//server tell everyone to delete this
		if (thisServer) {
			PlayerConnectionPacket newPacket = PlayerConnectionPacket();
			newPacket.type = Player_Disconnected;
			newPacket.playerID = playerID;
			thisServer->SendGlobalPacket(&newPacket, true);
		}

	}
	if (type == Handshake_Message) {
		std::cout << "Handshake_Message" << std::endl;
		selfID = ((HandshakePacket*)payload)->objectID;
		std::cout << name << " " << selfID << " recieved Handshake_Message" << std::endl;
		if (thisClient && localPlayer == nullptr) {
			std::cout << name << " " << selfID << " Creating localhost player "<< selfID << std::endl;
			localPlayer = SpawnPlayer(selfID,true);
		}
	}
}

void NetworkedGame::PlayerJoined(int playerID) {
	SpawnPlayer(playerID);
}

void NetworkedGame::PlayerLeft(int playerID) {
	//std::cout << name << " " << selfID << " deleting player " << playerID << std::endl;
	if (serverPlayers[playerID] != nullptr) {
		auto it = std::find(networkObjects.begin(), networkObjects.end(), serverPlayers[playerID]->GetNetworkObject());
		if (it != networkObjects.end()) {
			int index = it - networkObjects.begin();
			networkObjects.erase(networkObjects.begin()+index);
		}

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

	character->SetRenderObject(new RenderObject(&character->GetTransform(), charMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetRenderObject()->SetColour(Vector4(1, 0.9f, 0.8f, 1));

	character->GetPhysicsObject()->SetInverseMass(1);
	character->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(character);

	if (cameraFollow) {
		world->GetMainCamera()->SetFollow(&character->GetTransform());
		character->AttachedCamera();
	}

	character->GetPhysicsObject()->SetGravWeight(0);

	return character;
}