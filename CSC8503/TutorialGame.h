#pragma once
#include "GameTechRenderer.h"
#ifdef USEVULKAN
#include "GameTechVulkanRenderer.h"
#endif
#include "PhysicsSystem.h"

#include "StateGameObject.h"

namespace NCL {
	namespace CSC8503 {
		class Bullet;

		class TutorialGame {
		public:
			enum class InitMode {
				MIXED_GRID,
				CUBE_GRID,
				OBB_GRID,
				SPHERE_GRID,
				BRIDGE_TEST,
				BRIDGE_TEST_ANG,
				PERFORMANCE_TEST
			};

			TutorialGame();
			~TutorialGame();

			void InitWorld(InitMode mode = InitMode::MIXED_GRID);

			virtual void UpdateGame(float dt);
		protected:
			void InitialiseAssets();
			void InitialisePrefabs();

			void InitCamera();
			void UpdateKeys();

			void InitGameExamples();

			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims, bool axisAligned = true);
			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitBridgeConstraintTestWorld(int numLinks, float cubeDistance, float maxDistance, bool isOrientation);

			void InitDefaultFloor();

			GameObject* AddFloorToWorld(const Vector3& position);
			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f, bool axisAligned = true);
			GameObject* AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass = 10.0f);
			StateGameObject* AddStateObjectToWorld(const Vector3& position);

			GameObject* AddPlayerToWorld(const Vector3& position, bool cameraFollow = true);
			GameObject* AddEnemyToWorld(const Vector3& position);
			GameObject* AddBonusToWorld(const Vector3& position);
			GameObject* AddTriggerToWorld(const Vector3& position, float size);

			bool SelectObject();
			void MoveSelectedObject();
			void DebugObjectMovement();
			void LockedObjectMovement();

#ifdef USEVULKAN
			GameTechVulkanRenderer*	renderer;
#else
			GameTechRenderer* renderer;
#endif
			PhysicsSystem*		physics;
			GameWorld*			world;

			bool inSelectionMode;

			float		forceMagnitude;

			GameObject* selectionObject = nullptr;

			MeshGeometry*	capsuleMesh = nullptr;
			MeshGeometry*	cubeMesh	= nullptr;
			MeshGeometry*	sphereMesh	= nullptr;

			TextureBase*	basicTex	= nullptr;
			ShaderBase*		basicShader = nullptr;

			//Coursework Meshes
			MeshGeometry*	charMesh	= nullptr;
			MeshGeometry*	enemyMesh	= nullptr;
			MeshGeometry*	bonusMesh	= nullptr;

			Bullet* bulletPrefab = nullptr;

			//Coursework Additional functionality	
			GameObject* lockedObject	= nullptr;
			Vector3 lockedOffset		= Vector3(0, 14, 20);
			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}

			GameObject* objClosest = nullptr;
		};
	}
}

