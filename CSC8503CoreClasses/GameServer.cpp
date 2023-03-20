#include "GameServer.h"
#include "GameWorld.h"
#include "./enet/enet.h"
using namespace NCL;
using namespace CSC8503;

GameServer::GameServer(int onPort, int maxClients)	{
	port		= onPort;
	clientMax	= maxClients;
	clientCount = 0;
	netHandle	= nullptr;
	Initialise();
}

GameServer::~GameServer()	{
	Shutdown();
}

void GameServer::Shutdown() {
	SendGlobalPacket(BasicNetworkMessages::Shutdown,false);
	enet_host_destroy(netHandle);
	netHandle = nullptr;
}

bool GameServer::Initialise() {
	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = port;

	netHandle = enet_host_create(&address, clientMax, 1, 0, 0);

	if (!netHandle) {
		std::cout << __FUNCTION__ << " failed to create network handle!" << std::endl;
		return false;
	}
	return true;
}

bool GameServer::SendGlobalPacket(int msgID, bool isReliable) {
	GamePacket packet;
	packet.type = msgID;
	return SendGlobalPacket(&packet, isReliable);
}

bool GameServer::SendGlobalPacket(GamePacket* packet, bool isReliable){
	ENetPacket* dataPacket = enet_packet_create(packet, packet->GetTotalSize(), (isReliable?ENET_PACKET_FLAG_RELIABLE:0));
	enet_host_broadcast(netHandle, 0, dataPacket);
	return true;
}

bool GameServer::SendPacket(GamePacket* payload, int clientID, bool isReliable) {
	if (clientIDToPeer[clientID] == NULL)
		return false;
	ENetPacket* dataPacket = enet_packet_create(payload, payload->GetTotalSize(), 0);
	enet_peer_send(clientIDToPeer[clientID], 0, dataPacket);
	return true;
}

void GameServer::UpdateServer() {
	if (!netHandle) { return; }
	ENetEvent event;
	while (enet_host_service(netHandle, &event, 0) > 0) {
		int type = event.type;
		ENetPeer* p = event.peer;
		if (type == ENetEventType::ENET_EVENT_TYPE_CONNECT) {
			clientIDToPeer[nextClientID] = p;
			peerToClientID[p] = nextClientID;
			clientIDs.push_back(nextClientID);
			handshakeMap[nextClientID] = false;
			PlayerConnectionPacket packet = PlayerConnectionPacket();
			packet.type = Player_Connected;
			packet.playerID = nextClientID;

			ProcessPacket(&packet, nextClientID);

			std::cout << "Server: New client connected" << std::endl;
			nextClientID++;
		}
		else if (type == ENetEventType::ENET_EVENT_TYPE_DISCONNECT) {
			std::cout << "Server: A client has disconnected" << peerToClientID[p] << std::endl;
			PlayerConnectionPacket packet = PlayerConnectionPacket();
			packet.type = Player_Disconnected;
			packet.playerID = peerToClientID[p];
			ProcessPacket(&packet, peerToClientID[p]);

			DisconnectClient(peerToClientID[p]);

		}
		else if (type == ENetEventType::ENET_EVENT_TYPE_RECEIVE) {
			GamePacket * packet = (GamePacket*)event.packet->data;
			ProcessPacket(packet, peerToClientID[p]);
		}
		enet_packet_destroy(event.packet);
	}
}

void GameServer::DisconnectClient(int clientID)
{
	peerToClientID.erase(clientIDToPeer[clientID]);

	clientIDs.erase(std::remove(clientIDs.begin(), clientIDs.end(), clientID), clientIDs.end());
	clientIDToPeer.erase(clientID);
}

void GameServer::SetGameWorld(GameWorld &g) {
	gameWorld = &g;
}