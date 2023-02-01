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
			void UpdateStaticTree();

			void SetGlobalDamping(float d) {
				globalDamping = d;
			}

			void SetGravity(const Vector3& g);
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

			QuadTree<GameObject*> dynamicQuadTree;
			QuadTree<GameObject*> staticQuadTree;
			
			std::set<CollisionDetection::CollisionInfo> allTriggers;
			std::set<CollisionDetection::CollisionInfo> broadphaseTriggers;
			std::set<CollisionDetection::CollisionInfo> allCollisions;
			std::set<CollisionDetection::CollisionInfo> broadphaseCollisions;
			bool useBroadPhase		= true;
			int numCollisionFrames	= 5;
		};
	}
}

