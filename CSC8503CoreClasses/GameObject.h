#pragma once
#include "CollisionVolume.h"
#include "Transform.h"

#include <functional>

using std::vector;

namespace NCL::CSC8503 {
	class GameWorld;
	class NetworkObject;
	class PhysicsObject;
	class RenderObject;

	class GameObject	{
	public:
		typedef std::function<void(GameObject*)> overlap_func;

		GameObject(GameWorld& gameWorld, std::string name = "");

		GameObject(GameObject& other);
		GameObject& operator=(GameObject other);

		~GameObject();

		virtual void Update(float dt) {}

		void SetBoundingVolume(CollisionVolume* vol) {
			boundingVolume = vol;
		}

		const CollisionVolume* GetBoundingVolume() const {
			return boundingVolume;
		}

		bool IsActive() const {
			return isActive;
		}

		Transform& GetTransform() {
			return transform;
		}

		RenderObject* GetRenderObject() const {
			return renderObject;
		}

		PhysicsObject* GetPhysicsObject() const {
			return physicsObject;
		}

		NetworkObject* GetNetworkObject() const {
			return networkObject;
		}

		void SetRenderObject(RenderObject* newObject) {
			renderObject = newObject;
		}

		void SetPhysicsObject(PhysicsObject* newObject) {
			physicsObject = newObject;
		}

		const std::string& GetName() const {
			return name;
		}

		virtual void OnCollisionBegin(GameObject* otherObject) {
			if (OnCollisionBeginCallback) OnCollisionBeginCallback(otherObject);
		}

		virtual void OnCollisionEnd(GameObject* otherObject) {
			if (OnCollisionEndCallback) OnCollisionEndCallback(otherObject);
		}

		virtual void OnTriggerBegin(GameObject* otherObject) {
			std::cout << name << " Triggered with " << otherObject->name << "\n";
			if (OnTriggerBeginCallback) OnTriggerBeginCallback(otherObject);
		}

		virtual void OnTriggerEnd(GameObject* otherObject) {
			if (OnTriggerEndCallback) OnTriggerEndCallback(otherObject);
		}

		bool GetBroadphaseAABB(Vector3&outsize) const;

		void UpdateBroadphaseAABB();

		void SetWorldID(int newID) {
			worldID = newID;
		}

		int GetWorldID() const {
			return worldID;
		}

		void Delete() {
			markDelete = true;
		}
		bool IsMarkedDelete() {
			return markDelete;
		}

		overlap_func OnCollisionBeginCallback = nullptr;
		overlap_func OnCollisionEndCallback = nullptr;
		overlap_func OnTriggerBeginCallback = nullptr;
		overlap_func OnTriggerEndCallback = nullptr;
	protected:
		GameWorld& gameWorld;

		Transform transform;

		CollisionVolume* boundingVolume;
		PhysicsObject*   physicsObject;
		RenderObject*    renderObject;
		NetworkObject*   networkObject;

		bool isActive;
		int  worldID;
		std::string	name;

		Vector3 broadphaseAABB;
	private:
		bool markDelete = false;
	};
}
