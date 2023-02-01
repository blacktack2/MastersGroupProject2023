#pragma once
#include "BehaviourNode.h"
#include "BehaviourSelector.h"
#include "BehaviourSequence.h"
#include "BehaviourParallel.h"
#include "BehaviourAction.h"
#include "GameObject.h"

namespace NCL {
	namespace CSC8503 {
		class Bullet;
		class OrientationConstraint;
		class PositionConstraint;

		class PlayerObject : public GameObject {
		public:
			PlayerObject(GameWorld& gameWorld, int id, int& scoreCounter);
			~PlayerObject();

			virtual void Update(float dt) override;

			void AddPoints(int points);

			void CollisionWith(GameObject* other);
		protected:
			int id;
			BehaviourParallel behaviourRoot;
			BehaviourSelector groundAirSelector;
		private:
			void HandleGroundInput(float dt);
			void HandleGoatActions(float dt);

			void FireLasers();

			int& scoreCounter;

			std::set<int> collidedWith;

			GameObject* groundTrigger;
			GameObject* tounge;

			GameObject* grappledObject = nullptr;

			OrientationConstraint* groundOrientationConstraint;
			PositionConstraint* grappleConstraint = nullptr;

			int groundTriggerOverlaps = 0;
			bool isGrounded = false;

			float toungeMaxDistance = 10.0f;
			const Vector3 toungePos = Vector3(0.0f, 0.8f, -1.0f);
			Vector3 toungeContactPoint = Vector3(0);

			const Vector3 eyePosL = Vector3(-0.1f, 0.9f, -1.0f);
			const Vector3 eyePosR = Vector3( 0.1f, 0.9f, -1.0f);

			Vector3 laserForce = Vector3(0, 0, -1000);
			float laserFireRate = 0.05f;
			float laserDelay = 0.0f;
			float laserLifespan = 1.0f;

			float lastGoosed = 0.0f;
			const float gooseDelay = 2.0f;
		};
	}
}
