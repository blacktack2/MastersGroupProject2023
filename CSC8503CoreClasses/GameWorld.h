#pragma once
#include "CollisionDetection.h"
#include "GameObject.h"
#include "Light.h"
#include "QuadTree.h"
#include "Ray.h"
#include "StateMachine.h"

#include <functional>
#include <random>
#include <string>
#include <vector>

namespace NCL {
	class Camera;
	using Maths::Ray;
	namespace CSC8503 {
		class Constraint;
		class GameObject;


		typedef std::function<void(GameObject*)> GameObjectFunc;
		typedef std::function<void(Constraint*)> ConstraintFunc;
		typedef std::function<void(const Light&)> LightFunc;

		typedef std::vector<GameObject*>::const_iterator GameObjectIterator;
		typedef std::vector<Constraint*>::const_iterator ConstraintIterator;
		typedef std::vector<Light>::const_iterator LightIterator;

		class GameWorld {
		private:
			GameWorld();
			~GameWorld();
		public:
			static GameWorld& instance() {
				static GameWorld INSTANCE;
				return INSTANCE;
			}

			void Clear();
			void ClearAndErase();

			void AddGameObject(GameObject* o);
			void RemoveGameObject(GameObject* o, bool andDelete = false);

			void AddConstraint(Constraint* c);
			void RemoveConstraint(Constraint* c, bool andDelete = false);
			void RemoveConstraint(std::vector<Constraint*>::const_iterator c, bool andDelete = false);

			Light& AddLight(const Light& l);
			void RemoveLight(std::vector<Light>::const_iterator l);

			Camera* GetMainCamera() const {
				return mainCamera;
			}

			void ShuffleConstraints(bool state) {
				shuffleConstraints = state;
			}

			void ShuffleObjects(bool state) {
				shuffleObjects = state;
			}

			bool Raycast(Ray& r, RayCollision& closestCollision, bool closestObject = false, GameObject* ignore = nullptr);

			virtual void PreUpdateWorld();
			virtual void UpdateWorld(float dt);
			virtual void PostUpdateWorld();

			void UpdateStaticTree();
			void UpdateDynamicTree();

			void OperateOnStaticTree(QuadTreeNode::QuadTreeFunc func, const Vector2* subsetPos = nullptr, const Vector2* subsetSize = nullptr);
			void OperateOnDynamicTree(QuadTreeNode::QuadTreeFunc func, const Vector2* subsetPos = nullptr, const Vector2* subsetSize = nullptr);

			void OperateOnContents(GameObjectFunc f);
			void OperateOnConstraints(ConstraintFunc f);
			void OperateOnLights(LightFunc f);

			void GetObjectIterators(GameObjectIterator& first, GameObjectIterator& last) const;
			void GetConstraintIterators(ConstraintIterator& first, ConstraintIterator& last) const;
			void GetLightIterators(LightIterator& first, LightIterator& last) const;

			inline int GetWorldStateID() const {
				return worldStateCounter;
			}

			inline float GetRunTime() const {
				return runTime;
			}
		protected:
			std::vector<GameObject*> gameObjects;
			std::vector<Constraint*> constraints;
			std::vector<Light> lights;

			QuadTree dynamicQuadTree;
			QuadTree staticQuadTree;

			Camera* mainCamera;

			bool shuffleConstraints;
			bool shuffleObjects;
			int worldIDCounter;
			int worldStateCounter;

			float runTime;
		};
	}
}

