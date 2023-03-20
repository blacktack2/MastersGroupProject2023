#pragma once
#include "CollisionDetection.h"
#include "GameObject.h"
#include "Light.h"
#include "QuadTree.h"
#include "Ray.h"
//#include "StateMachine.h" needs to be implemented

#include <functional>
#include <random>
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
#ifdef x64
			void AddNetworkObject(NetworkObject* o);
			void RemoveNetworkObject(NetworkObject* o);
			std::vector<NetworkObject*> GetNetworkObjects() {
				return networkObjects;
			}
#endif //x64

			PointLight&       AddPointLight(const Vector3& position, const Vector4& colour, float radius);
			SpotLight&        AddSpotLight(const Vector3& position, const Vector4& direction, const Vector4& colour, float radius, float angle);
			DirectionalLight& AddDirectionalLight(const Vector3& direction, const Vector4& colour);
			void RemoveLight(LightIterator l);
			void ClearLight();

			Camera* GetMainCamera() const {
				return mainCamera;
			}
			void InitCameras() const {
				InitCamera(playerCamera1);
				InitCamera(playerCamera2);
				InitCamera(playerCamera3);
				InitCamera(playerCamera4);

			}
			void InitCamera(Camera* cam) const {
				if (!cam)
					return;
				cam->SetNearPlane(0.1f);
				cam->SetFarPlane(1000.0f);
				cam->SetPitch(-15.0f);
				cam->SetYaw(315.0f);
				cam->SetPosition(Vector3(-60, 40, 60));
				cam->SetFollow(nullptr);
			}

			Camera* GetCamera(int n) const {
#ifdef x64
				switch (n)
				{
				case 0:
					return playerCamera4;	// this is HARDCODED! fix this later!
				case 1:
					return playerCamera1;
				case 2:
					return playerCamera2;
				case 3:
					return playerCamera3;
				case 4:
					return playerCamera4;
				default:
					return nullptr;		// wrong input
				}
#endif // x64
#ifdef _ORBIS
				switch (n)
				{
				case 0:
					return playerCamera1;	// this is HARDCODED! fix this later!
				case 1:
					return playerCamera2;
				case 2:
					return playerCamera3;
				case 3:
					return playerCamera4;
				default:
					return nullptr;		// wrong input
				}
#endif // _ORBIS
				
			}

			Camera* SetMainCamera(int n)
			{
#ifdef x64
				switch (n)
				{
				case 1:
					mainCamera = playerCamera1;
					return mainCamera;
				case 2:
					mainCamera = playerCamera2;
					return mainCamera;
				case 3:
					mainCamera = playerCamera3;
					return mainCamera;
				case 4:
					mainCamera = playerCamera4;
					return mainCamera;
				default:
					return nullptr;		// wrong input
				}
#endif // x64

#ifdef _ORBIS
				switch (n)
				{
				case 0:
					mainCamera = playerCamera1;
					return mainCamera;
				case 1:
					mainCamera = playerCamera2;
					return mainCamera;
				case 2:
					mainCamera = playerCamera3;
					return mainCamera;
				case 3:
					mainCamera = playerCamera4;
					return mainCamera;
				default:
					return nullptr;		// wrong input
				}
#endif // _ORBIS

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
			/*std::vector<NetworkObject*> networkObjects; needs to be implemented */
			std::vector<std::unique_ptr<Light>> lights;

			QuadTree dynamicQuadTree;
			QuadTree staticQuadTree;

			Camera* mainCamera;
			Camera* playerCamera1 = nullptr;
			Camera* playerCamera2 = nullptr;
			Camera* playerCamera3 = nullptr;
			Camera* playerCamera4 = nullptr;

			bool shuffleConstraints;
			bool shuffleObjects;
			int worldIDCounter;
			int worldStateCounter;

			float runTime;
			float deltaTime = 0.0f;		// Used by Animation
		};
	}
}

