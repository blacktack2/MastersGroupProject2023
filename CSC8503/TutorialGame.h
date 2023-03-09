/**
 * @file   TutorialGame.h
 * @brief  
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   March 2023
 */
#pragma once
#include "Vector3.h"

#include <memory>

namespace NCL {
	class DebugViewPoint;
	class InputKeyMap;
}

using namespace NCL::Maths;

namespace NCL::CSC8503 {
	class GameGridManager;
	class GameStateManager;
	class GameTechRenderer;
	class GameWorld;
	class MenuManager;

	class GameLevel;
	class PhysicsSystem;

	class GameObject;

	class GameGrid;
	class Boss;
	class BossBehaviorTree;
	class PlayerObject;

	class Light;

	class TutorialGame {
	public:
		TutorialGame();
		~TutorialGame();

		virtual void Clear();
		virtual void StartLevel();

		void InitWorld();

		virtual void UpdateGame(float dt);

		bool IsQuit();
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

		DebugViewPoint&   debugViewPoint;
		GameGridManager&  gridManager;
		GameStateManager& gameStateManager;
		GameTechRenderer& renderer;
		GameWorld&        gameWorld;
		InputKeyMap&      keyMap;
		MenuManager&      menuManager;

		std::unique_ptr<PhysicsSystem> physics;

		Light* sunLight = nullptr;
		PlayerObject* player = nullptr;
		GameLevel* gameLevel = nullptr;

		GameGrid* gameGrid = nullptr;
		Boss* testingBoss = nullptr;

		float interval = 0.0f;
	};
}
