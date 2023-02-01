#pragma once
#include "EnemyObject.h"
#include "GameTechRenderer.h"
#ifdef USEVULKAN
#include "GameTechVulkanRenderer.h"
#endif
#include "NPCObject.h"
#include "PhysicsSystem.h"
#include "PlayerObject.h"
#include "StateGameObject.h"

namespace NCL {
	namespace CSC8503 {
		class Bullet;
		class Maze;

		class TutorialGame {
		public:
			enum class InitMode {
				MAZE,
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

			void InitWorld(InitMode mode = InitMode::MAZE);

			virtual void UpdateGame(float dt);

			bool IsQuit() {
				return gameState == GameState::Quit;
			}
		protected:
			enum class GameState {
				OnGoing,
				Paused,
				Win,
				Lose,
				Quit,
			};

			void InitialiseAssets();
			void InitialisePrefabs();

			void InitCamera();
			void UpdateKeys();

			void InitGameExamples();

			void InitMazeWorld(int numRows, int numCols, float size);
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

			PlayerObject* AddPlayerToWorld(const Vector3& position, bool cameraFollow = true);
			EnemyObject* AddEnemyToWorld(const Vector3& position, NavigationMap& navMap);
			NPCObject* AddNPCToWorld(const Vector3& position);
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

			GameState gameState;
			bool inSelectionMode;

			float		forceMagnitude;

			Maze* mazes = nullptr;

			GameObject* selectionObject = nullptr;

			MeshGeometry*	capsuleMesh = nullptr;
			MeshGeometry*	cubeMesh    = nullptr;
			MeshGeometry*	sphereMesh  = nullptr;

			TextureBase*	basicTex    = nullptr;
			ShaderBase*		basicShader = nullptr;

			//Coursework Meshes
			MeshGeometry*	charMesh = nullptr;
			MeshGeometry*	enemyMesh = nullptr;
			MeshGeometry*	npcMesh	  = nullptr;
			MeshGeometry*	bonusMesh = nullptr;

			Bullet* bulletPrefab = nullptr;

			//Coursework Additional functionality	
			GameObject* lockedObject	= nullptr;
			Vector3 lockedOffset		= Vector3(0, 14, 20);
			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}

			GameObject* objClosest = nullptr;

			PlayerObject* player = nullptr;

			int score;
		};
	}
}

