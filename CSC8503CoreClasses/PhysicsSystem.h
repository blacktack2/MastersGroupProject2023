/**
 * @file   PhysicsSystem.h
 * @brief  A class that handles physics interactions and calculations after collision detection.
 * 
 * @author Shashwat Kashyap
 * @author Stuart Lewis
 * @author Harry Brettell
 * @author Rich Davidson
 * @date   March 2023
 */
#pragma once
#include "GameWorld.h"
#include "QuadTree.h"

namespace NCL {
	namespace CSC8503 {
		class PhysicsSystem	{
		public:
			PhysicsSystem(GameWorld& g);
			~PhysicsSystem();

			void Clear();

			void Update(float dt);

			void SetGlobalDamping(float d) {
				globalDamping = d;
			}

			void SetGravity(const Vector3& g);
			void ClearObjTriggers(GameObject* target);

		protected:
			void BasicCollisionDetection();
			void BroadPhase();
			void NarrowPhase();

			void ClearForces();

			void IntegrateAccel(float dt);
			void IntegrateVelocity(float dt);

			void UpdateConstraints(float dt);

			void UpdateCollisionList();
			void UpdateObjectAABBs();

			void ImpulseResolveCollision(GameObject& a , GameObject&b, CollisionDetection::ContactPoint& p) const;

			GameWorld& gameWorld;

			Vector3 gravity;
			float	dTOffset;
			float	globalDamping;
			
			std::set<CollisionDetection::CollisionInfo> allTriggers;
			std::set<CollisionDetection::CollisionInfo> broadphaseTriggers;
			std::set<CollisionDetection::CollisionInfo> allCollisions;
			std::set<CollisionDetection::CollisionInfo> broadphaseCollisions;
			bool useBroadPhase		= true;
			int numCollisionFrames	= 5;
		};
	}
}

