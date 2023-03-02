#include "CollisionDetection.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "NetworkObject.h"
#include "PhysicsObject.h"
#include "RenderObject.h"

using namespace NCL::CSC8503;

GameObject::GameObject(std::string objectName) : gameWorld(GameWorld::instance()), transform(this) {
	name			= objectName;
	worldID			= -1;
	isActive		= true;
	boundingVolume	= nullptr;
	physicsObject	= nullptr;
	renderObject	= nullptr;
	networkObject	= nullptr;
}

GameObject::GameObject(GameObject& other) : gameWorld(GameWorld::instance()), transform(this) {
	transform = other.transform;
	boundingVolume = other.boundingVolume == nullptr ? nullptr : CollisionVolume::Clone(*other.boundingVolume);
	physicsObject  = other.physicsObject  == nullptr ? nullptr : new PhysicsObject(*other.physicsObject, &transform);
	renderObject   = other.renderObject   == nullptr ? nullptr : new RenderObject(*other.renderObject, &transform);
	networkObject  = other.networkObject  == nullptr ? nullptr : new NetworkObject(*other.networkObject);

	isActive = other.isActive;
	worldID = -1;
	name = other.name + "(Copy)";

	broadphaseAABB = other.broadphaseAABB;
}

GameObject::~GameObject()	{
	if (networkObject != nullptr) {
		gameWorld.RemoveNetworkObject(networkObject);
		delete networkObject;
	}
		
	delete boundingVolume;
	delete physicsObject;
	delete renderObject;
}

bool GameObject::GetBroadphaseAABB(Vector3&outSize) const {
	if (!boundingVolume) {
		return false;
	}
	outSize = broadphaseAABB;
	return true;
}

void GameObject::UpdateBroadphaseAABB() {
	if (!boundingVolume) {
		return;
	}
	if (boundingVolume->type == VolumeType::AABB) {
		broadphaseAABB = ((AABBVolume&) *boundingVolume).GetHalfDimensions();
	} else if (boundingVolume->type == VolumeType::OBB) {
		Matrix3 mat = Matrix3(transform.GetGlobalOrientation());
		mat = mat.Absolute();
		Vector3 halfSizes = ((OBBVolume&) *boundingVolume).GetHalfDimensions();

		broadphaseAABB = mat * halfSizes;
	} else if (boundingVolume->type == VolumeType::Sphere) {
		float r = ((SphereVolume&) *boundingVolume).GetRadius();

		broadphaseAABB = Vector3(r, r, r);
	} else if (boundingVolume->type == VolumeType::Capsule) {
		Matrix3 mat = Matrix3(transform.GetGlobalOrientation());
		mat = mat.Absolute();
		float r = ((CapsuleVolume&) *boundingVolume).GetRadius();
		float h = ((CapsuleVolume&) *boundingVolume).GetHalfHeight();

		broadphaseAABB = mat * Vector3(r, h, r);
	}
}
