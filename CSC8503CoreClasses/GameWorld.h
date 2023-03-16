#pragma once
#include "CollisionDetection.h"
#include "GameObject.h"
#include "Light.h"
#include "QuadTree.h"
#include "Ray.h"
#include "StateMachine.h"

#include <functional>
#include <random>
#include <memory>
#include <string>
#include <vector>

#define CAM_COUNT 4

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
		typedef std::vector<std::unique_ptr<Light>>::const_iterator LightIterator;

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

			void AddNetworkObject(NetworkObject* o);
			void RemoveNetworkObject(NetworkObject* o);
			std::vector<NetworkObject*> GetNetworkObjects() {
				return networkObjects;
			}

			PointLight&       AddPointLight(const Vector3& position, const Vector4& colour, float radius);
			SpotLight&        AddSpotLight(const Vector3& position, const Vector4& direction, const Vector4& colour, float radius, float angle);
			DirectionalLight& AddDirectionalLight(const Vector3& direction, const Vector4& colour);
			void RemoveLight(LightIterator l);
			void ClearLight();

			Camera* GetMainCamera() const;
			void InitCameras() const;
			void InitCamera(Camera* cam) const;
			Camera* GetCamera(int n) const;
			void UpdateCamera(float dt);
			Camera* SetMainCamera(int n);
			int GetMainCameraIndex() {
				return mainCameraIndex;
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

			inline float GetDeltaTime() const {
				return deltaTime;
			}
		protected:
			std::vector<GameObject*> gameObjects;
			std::vector<Constraint*> constraints;
			std::vector<NetworkObject*> networkObjects;
			std::vector<std::unique_ptr<Light>> lights;

			QuadTree dynamicQuadTree;
			QuadTree staticQuadTree;

			int mainCameraIndex = 1;
			//Check if needed
			std::unique_ptr<Camera> playerCamera1 = nullptr;
			std::unique_ptr<Camera> playerCamera2 = nullptr;
			std::unique_ptr<Camera> playerCamera3 = nullptr;
			std::unique_ptr<Camera> playerCamera4 = nullptr;

			Camera* mainCamera;
			Camera* cameras[CAM_COUNT];

			bool shuffleConstraints;
			bool shuffleObjects;
			int worldIDCounter;
			int worldStateCounter;

			float runTime;
			float deltaTime = 0.0f;		// Used by Animation
		};
	}
}

