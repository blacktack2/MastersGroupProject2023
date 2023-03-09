/**
 * @file   TutorialGame.h
 * @brief  
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   March 2023
 */
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

namespace NCL::CSC8503 {
	class Bullet;
	class Maze;

	class TutorialGame {
	public:
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

		void InitCamera();
		void InitGameExamples();

		void InitDefaultFloor();

		GameObject* AddFloorToWorld(const Vector3& position);

		PlayerObject* AddPlayerToWorld(const Vector3& position, bool cameraFollow = true);

		Boss* AddBossToWorld(const Vector3& position, Vector3 dimensions, float inverseMass);
		void BuildLevel();
		void UpdateLevel();

#ifdef USEVULKAN
		GameTechVulkanRenderer* renderer;
#else
		GameTechRenderer* renderer;
#endif
		PhysicsSystem* physics;
		GameWorld* world;

		InputKeyMap& keyMap = InputKeyMap::instance();

		Light* sunLight;

		GameStateManager* gameStateManager;
		MenuManager* menuManager;

		PlayerObject* player = nullptr;

		DebugViewPoint* debugViewPoint;

		GameLevel* gameLevel = nullptr;
		float interval = 0.0f;
		GameGrid* gameGrid = nullptr;
		Boss* testingBoss = nullptr;
		BossBehaviorTree* testingBossBehaviorTree = nullptr;

		GameGridManager* gridManager;
	};
}
