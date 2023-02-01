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
		class Constraint;

		class PlayerObject : public GameObject {
		public:
			PlayerObject(GameWorld& gameWorld, int id, Bullet& bulletPrefab);
			~PlayerObject();

			virtual void Update(float dt) override;
		protected:
			int id;
			BehaviourParallel behaviourRoot;
			BehaviourSelector groundAirSelector;
		private:
			void HandleGroundInput(float dt);
			void HandleGoatActions(float dt);

			void FireLasers();

			Bullet& bulletPrefab;

			GameObject* groundTrigger;

			Constraint* groundOrientationConstraint;

			int groundTriggerOverlaps = 0;
			bool isGrounded = false;

			const Vector3 eyePosL = Vector3(-0.1f, 0.9f, -1.0f);
			const Vector3 eyePosR = Vector3( 0.1f, 0.9f, -1.0f);

			Vector3 laserForce = Vector3(0, 0, -1000);
			float laserFireRate = 0.05f;
			float laserDelay = 0.0f;
			float laserLifespan = 1.0f;
		};
	}
}
