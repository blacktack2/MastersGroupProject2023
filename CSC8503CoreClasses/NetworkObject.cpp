#include "NetworkObject.h"
#include "Debug.h"
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
	object->GetTransform().SetPosition(oldPos + (newPos - oldPos)*1.02);
	
}
bool NetworkObject::ReadDeltaPacket(DeltaPacket &p, float dt) {
	if (p.fullID != lastFullState.stateID) {
		return false;
	}
	UpdateStateHistory(p.fullID);

	Vector3 fullPos = lastFullState.position;
	Quaternion fullOrientation = lastFullState.orientation;

	fullPos.x += p.pos[0] * deltaPosDivisorInverse;
	fullPos.y += p.pos[1] * deltaPosDivisorInverse;
	fullPos.z += p.pos[2] * deltaPosDivisorInverse;

	fullOrientation.x += ((float)p.orientation[0]) * deltaOrientationDivisorInverse;
	fullOrientation.y += ((float)p.orientation[1]) * deltaOrientationDivisorInverse;
	fullOrientation.z += ((float)p.orientation[2]) * deltaOrientationDivisorInverse;
	fullOrientation.w += ((float)p.orientation[3]) * deltaOrientationDivisorInverse;

	lastDeltaState = NetworkState();
	lastDeltaState.position = fullPos;
	lastDeltaState.orientation = fullOrientation;
	Debug::DrawLine(lastDeltaState.position, lastDeltaState.position + Vector3(0, 0.1, 0), Debug::YELLOW, 5.0f);


	return true;
}

bool NetworkObject::ReadFullPacket(FullPacket &p, float dt) {
	if (p.fullState.stateID < lastFullState.stateID) {
		return false; //ignore 'old' packet
	}
	lastFullState = p.fullState;

	lastDeltaState = p.fullState;

	Debug::DrawLine(p.fullState.position, p.fullState.position + Vector3(0,0.1,0), Debug::BLUE, 5.0f);

	stateHistory.emplace_back(lastFullState);

	smoothFrameCount = smoothFrameTotal;
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

	dp->pos[0] = (int) (currentPos.x * deltaPosDivisor);
	dp->pos[1] = (int) (currentPos.y * deltaPosDivisor);
	dp->pos[2] = (int) (currentPos.z * deltaPosDivisor);

	dp->orientation[0] = (int)(currentOrientation.x * deltaOrientationDivisor);
	dp->orientation[1] = (int)(currentOrientation.y * deltaOrientationDivisor);
	dp->orientation[2] = (int)(currentOrientation.z * deltaOrientationDivisor);
	dp->orientation[3] = (int)(currentOrientation.w * deltaOrientationDivisor);

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

void NetworkObject::UpdateDelta(float dt) {
	object.GetTransform().SetPosition(Vector3::Lerp(object.GetTransform().GetGlobalPosition(), lastDeltaState.position, dt * 10));
	object.GetTransform().SetOrientation(Quaternion::Lerp(object.GetTransform().GetGlobalOrientation(), lastDeltaState.orientation, 0.2));
}
