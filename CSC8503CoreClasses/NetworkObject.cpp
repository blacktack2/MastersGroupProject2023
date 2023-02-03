#include "NetworkObject.h"
#include "./enet/enet.h"
using namespace NCL;
using namespace CSC8503;

NetworkObject::NetworkObject(GameObject& o, int id) : object(o)	{
	deltaErrors = 0;
	fullErrors  = 0;
	networkID   = id;
	object.SetNetworkObject(this);
}

NetworkObject::~NetworkObject()	{
}

bool NetworkObject::ReadPacket(GamePacket& p, float dt) {

	if (p.type == Delta_State) {
		if (networkID != ((DeltaPacket*)& p)->objectID)
			return false;
		return ReadDeltaPacket((DeltaPacket&)p, dt);
	}
	if (p.type == Full_State) {
		if (networkID != ((FullPacket*)& p)->objectID)
			return false;
		return ReadFullPacket((FullPacket&)p, dt);
	}
	return false; //this isn't a packet we care about!
}

bool NetworkObject::WritePacket(GamePacket** p, bool deltaFrame, int stateID) {
	if (deltaFrame) {
		if (!WriteDeltaPacket(p, stateID)) {
			return WriteFullPacket(p);
		}
		return true;
	}
	return WriteFullPacket(p);
}
//Client objects recieve these packets

void moveObject(float dt, Vector3 newPos, Vector3 oldPos, GameObject* object) {
	object->GetTransform().SetPosition(newPos);
	/*
	if ((newPos - oldPos).Length() < 1.0f) {
		//object->MoveTowards(dt, newPos, 10.0f);
	}
	else if ((newPos - oldPos).Length() < 10.0f) {
		//object->MoveTowards(dt, newPos, 10.0f);
	}
	else {
		object->GetTransform().SetPosition(newPos);
	}
	*/
}
bool NetworkObject::ReadDeltaPacket(DeltaPacket &p, float dt) {
	if (p.fullID != lastFullState.stateID) {
		return false;
	}
	UpdateStateHistory(p.fullID);

	Vector3 fullPos = lastFullState.position;
	Quaternion fullOrientation = lastFullState.orientation;

	fullPos.x += p.pos[0] * 0.01;
	fullPos.y += p.pos[1] * 0.01;
	fullPos.z += p.pos[2] * 0.01;

	fullOrientation.x += ((float)p.orientation[0]) * 0.1;
	fullOrientation.y += ((float)p.orientation[1]) * 0.1;
	fullOrientation.z += ((float)p.orientation[2]) * 0.1;
	fullOrientation.w += ((float)p.orientation[3]) * 0.1;
	//fullOrientation.y += p.orientation[1];
	//fullOrientation.z += p.orientation[2];
	//fullOrientation.x += p.orientation[0];
	//fullOrientation.w += p.orientation[3];

	moveObject(dt, fullPos, object.GetTransform().GetGlobalPosition(), &object);

	//object.GetTransform().SetPosition(fullPos);
	object.GetTransform().SetOrientation(fullOrientation);

	return true;
}

bool NetworkObject::ReadFullPacket(FullPacket &p, float dt) {
	if (p.fullState.stateID < lastFullState.stateID) {
		return false; //ignore 'old' packet
	}
	lastFullState = p.fullState;

	//interpolate the position ?
	/*
	Vector3 position = object.GetTransform().GetGlobalPosition();
	object.GetPhysicsObject()->SetLinearVelocity(lastFullState.linearVelocity);
	Vector3 linearVel = object.GetPhysicsObject()->GetLinearVelocity();
	position += linearVel * dt;
	transform.SetPosition(position);
	*/

	//object.GetTransform().SetPosition(lastFullState.position);
	moveObject(dt, lastFullState.position, object.GetTransform().GetGlobalPosition(), &object);

	object.GetTransform().SetOrientation(lastFullState.orientation);
	//object.GetPhysicsObject()->SetLinearVelocity(lastFullState.linearVelocity);
	object.GetPhysicsObject()->SetAngularVelocity(lastFullState.angularVelocity);

	stateHistory.emplace_back(lastFullState);

	return true; 
}

bool NetworkObject::WriteDeltaPacket(GamePacket**p, int stateID) {
	DeltaPacket* dp = new DeltaPacket();
	NetworkState state;
	if (!GetNetworkState(stateID, state)) {
		return false; // can't delta
	}

	dp->fullID = stateID;
	dp->objectID = networkID;
	Vector3 currentPos = object.GetTransform().GetGlobalPosition();
	Quaternion currentOrientation = object.GetTransform().GetGlobalOrientation();
	currentPos -= state.position;
	currentOrientation -= state.orientation;

	dp->pos[0] = (int) currentPos.x * 100;
	dp->pos[1] = (int) currentPos.y * 100;
	dp->pos[2] = (int) currentPos.z * 100;

	dp->orientation[0] = (short int)(currentOrientation.x * 10);
	dp->orientation[1] = (short int)(currentOrientation.y * 10);
	dp->orientation[2] = (short int)(currentOrientation.z * 10);
	dp->orientation[3] = (short int)(currentOrientation.w * 10);

	*p = dp;


	return true;
}

bool NetworkObject::WriteFullPacket(GamePacket**p) {
	FullPacket* fp = new FullPacket();

	fp->objectID = networkID;
	fp->fullState.position = object.GetTransform().GetGlobalPosition();
	fp->fullState.orientation = object.GetTransform().GetGlobalOrientation();
	fp->fullState.stateID = lastFullState.stateID++;
	fp->fullState.linearVelocity = object.GetPhysicsObject()->GetLinearVelocity();
	fp->fullState.angularVelocity = object.GetPhysicsObject()->GetAngularVelocity();

	*p = fp;

	stateHistory.emplace_back(fp->fullState);

	return true;
}

NetworkState& NetworkObject::GetLatestNetworkState() {
	return lastFullState;
}

bool NetworkObject::GetNetworkState(int stateID, NetworkState& state) {
	if (stateID == -1)
		return false;
	for (auto i = stateHistory.begin(); i < stateHistory.end(); ++i) {
		if ((*i).stateID == stateID) {
			state = (*i);
			return true;
		}
	}
	return false;
}

void NetworkObject::UpdateStateHistory(int minID) {
	for (auto i = stateHistory.begin(); i < stateHistory.end();) {
		if ((*i).stateID < minID) {
			i = stateHistory.erase(i);
		}
		else {
			++i;
		}
	}
}