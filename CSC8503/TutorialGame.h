#pragma once
#include "GameTechRenderer.h"
#ifdef USEVULKAN
#include "GameTechVulkanRenderer.h"
#endif
#include "PhysicsSystem.h"
#include "PlayerObject.h"
#include "StateGameObject.h"
#include "DebugViewPoint.h"

#include "GameGridManager.h"

#include "Boss.h"	
#include "Obstacle.h"
#include "GameLevel.h"


#include "GameStateManager.h"
#include "MenuManager.h"

#include "MeshAnimation.h";




namespace NCL {
	namespace CSC8503 {
		class Bullet;
		class Maze;

		class TutorialGame {
		public:
			enum class InitMode {
				EMPTY,
				MAZE,
				MIXED_GRID,
				CUBE_GRID,
				OBB_GRID,
				SPHERE_GRID,
				BRIDGE_TEST,
				BRIDGE_TEST_ANG,
				PERFORMANCE_TEST,
				AUDIO_TEST//added for audio testing
			};

			TutorialGame();
			~TutorialGame();

			virtual void Clear();
			virtual void StartLevel();
			void InitWorld();

			virtual void UpdateGame(float dt);

			bool IsQuit() {
				return gameStateManager->GetGameState() == GameState::Quit;
			}
		protected:

			void UpdateGameCore(float dt);
			virtual void ProcessState();

			void SetCameraFollow(PlayerObject* p);

			void InitialiseAssets();
			void InitialisePrefabs();

			void InitCamera();
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

			PlayerObject* AddPlayerToWorld(int playerID, const Vector3& position);

			Boss* AddBossToWorld(const Vector3& position, Vector3 dimensions, float inverseMass);
			void BuildLevel();
			void UpdateLevel();
			GameObject* AddBonusToWorld(const Vector3& position);
			GameObject* AddTriggerToWorld(const Vector3& position, float size);

			void InitaliseAnimationAssets();
			MeshMaterial* maleguardMaterial = nullptr;
			MeshAnimation* maleguardAnim = nullptr;
			MeshGeometry* maleguardMesh = nullptr;
			std::vector<OGLTexture*>  maleguardMatTextures;

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

			paintHell::InputKeyMap& keyMap = paintHell::InputKeyMap::instance();

			Light* sunLight;

			GameStateManager* gameStateManager;
			MenuManager* menuManager;

			bool inSelectionMode;

			float		forceMagnitude;

			Maze* mazes = nullptr;

			GameObject* selectionObject = nullptr;

			MeshGeometry*	capsuleMesh = nullptr;
			MeshGeometry*	cubeMesh    = nullptr;
			MeshGeometry* sphereMesh = nullptr;

			TextureBase*	basicTex    = nullptr;
			TextureBase*	pillarTex    = nullptr;
			TextureBase*	healingKitTex = nullptr;

			//Coursework Meshes
			MeshGeometry*	charMesh = nullptr;
			MeshGeometry*	enemyMesh = nullptr;
			MeshGeometry*	npcMesh	  = nullptr;
			MeshGeometry*	bonusMesh = nullptr;

			//Coursework Additional functionality	
			GameObject* lockedObject	= nullptr;
			Vector3 lockedOffset		= Vector3(0, 14, 20);
			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}

			GameObject* objClosest = nullptr;

			PlayerObject* player = nullptr;
			PlayerObject* player2 = nullptr;
			PlayerObject* player3 = nullptr;
			PlayerObject* player4 = nullptr;

			int score;

			paintHell::debug::DebugViewPoint* debugViewPoint;

			GameLevel* gameLevel = nullptr;	/////////
			float interval = 0.0f;			/////////
			GameGrid* gameGrid = nullptr;	/////////
			//GameObject* floor = nullptr;	/////////
			Boss* testingBoss = nullptr;   /////////
			BossBehaviorTree* testingBossBehaviorTree = nullptr;   /////////

			//GameGrid stuff
			GameGridManager* gridManager;
			float wallTimer = 0.0f;

			//menu
			bool isclicked = false;
		};
	}
}

