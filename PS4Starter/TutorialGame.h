#pragma once
//#include "EnemyObject.h"
//#include "GameTechRenderer.h"
//#ifdef USEVULKAN
//#include "GameTechVulkanRenderer.h"
//#endif
//#include "NPCObject.h"

//#include "PlayerObject.h"
//#include "StateGameObject.h"
//#include "DebugViewPoint.h"

//#include "GameGridManager.h"
//
//#include "Boss.h"	
//#include "Obstacle.h"
//#include "GameLevel.h"
//
//#include "GameStateManager.h"

//#include "../Common/PhysicsSystem.h"
//#include "../Common/GameObject.h"
//#include "../Common/MeshGeometry.h"

#ifdef _ORBIS
#include "PS4Renderer.h"
#endif // _ORBIS

#include "../Common/TextureBase.h"
#include "GameLevel.h"
#include "InputKeyMap.h"

namespace NCL {

	namespace CSC8503 {
		class Bullet;
		class Maze;
		class PhysicsSystem;
		class GameObject;
		class GameGridManager;
		class GameStateManager;
		class MeshGeometry;
		class PlayerObject;
		class Boss;
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

			void InitWorld(InitMode mode = InitMode::EMPTY);

			virtual void UpdateGame(float dt);

			/*bool IsQuit() {
				return gameStateManager->GetGameState() == GameState::Quit;
			}*/
		protected:

			//void UpdateStateOngoing(float dt);
			float timePassed = 0.0f;
			void InitialiseAssets();
			/*void InitialisePrefabs();*/

			void InitCamera();
			//void UpdateKeys();
			//void InitGameExamples();
			void InitDefaultFloor();

			GameObject* AddFloorToWorld(const Vector3& position);
			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f, bool axisAligned = true);
			GameObject* AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass = 10.0f);
			//StateGameObject* AddStateObjectToWorld(const Vector3& position);

			PlayerObject* AddPlayerToWorld(const Vector3& position, bool cameraFollow = true);

			GameGridManager& gridManager;
			GameStateManager& gameStateManager;

			void SetCameraFollow(PlayerObject* p);
			//EnemyObject* AddEnemyToWorld(const Vector3& position, NavigationMap& navMap);

			Boss* AddBossToWorld(const Vector3& position, Vector3 dimensions, float inverseMass);		/////////
			//HealingKit* UpdateHealingKit();																/////////
			void BuildLevel();																			/////////
			void UpdateLevel();																			/////////
			
			//void BossTarget();
			void ProcessState();

			/*NPCObject* AddNPCToWorld(const Vector3& position);*/
			/*GameObject* AddBonusToWorld(const Vector3& position);
			GameObject* AddTriggerToWorld(const Vector3& position, float size);

			bool SelectObject();
			void MoveSelectedObject();
			void DebugObjectMovement();
			void LockedObjectMovement();*/

#ifdef USEVULKAN
			GameTechVulkanRenderer* renderer;
#elif _ORBIS
			NCL::PS4::PS4Renderer* renderer;
#else
			NCL::CSC8503::GameTechRenderer* renderer;
#endif
			PhysicsSystem* physics;
			GameWorld* world;


			InputKeyMap& keyMap = InputKeyMap::instance();

			Light* sunLight;

			/*GameStateManager* gameStateManager;*/

			bool inSelectionMode;

			float		forceMagnitude;

			Maze* mazes = nullptr;

			GameObject* selectionObject = nullptr;

			MeshGeometry* capsuleMesh = nullptr;
			MeshGeometry* cubeMesh = nullptr;
			MeshGeometry* sphereMesh = nullptr;

			TextureBase* basicTex = nullptr;
			TextureBase* healingKitTex = nullptr;		/////////
			TextureBase* inkableTex = nullptr;		/////////
			TextureBase* noiseTex = nullptr;		/////////
			//ShaderBase* inkableShader = nullptr;		/////////
			//ShaderBase* basicShader = nullptr;

			//Coursework Meshes
			MeshGeometry* charMesh = nullptr;
			MeshGeometry* enemyMesh = nullptr;
			MeshGeometry* npcMesh = nullptr;
			MeshGeometry* bonusMesh = nullptr;
			MeshGeometry* fenceMesh = nullptr;
			MeshGeometry* pillarMesh = nullptr;

			Bullet* bulletPrefab = nullptr;

			//Coursework Additional functionality	
			GameObject* lockedObject = nullptr;
			Vector3 lockedOffset = Vector3(0, 14, 20);
			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}

			GameObject* objClosest = nullptr;

			PlayerObject* player = nullptr;

			int score;

			/*paintHell::debug::DebugViewPoint* debugViewPoint;*/

			GameLevel* gameLevel = nullptr;	/////////
			//float interval = 0.0f;			/////////
			GameObject* floor = nullptr;	/////////
			Boss* boss = nullptr;   /////////
			//BossBehaviorTree* testingBossBehaviorTree = nullptr;   /////////

			////GameGrid stuff
			//GameGridManager* gridManager;
			
			float interval = 0.0f;

			
		};
	}
}

