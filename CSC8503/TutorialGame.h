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


enum ControllerType;
namespace NCL {
	namespace CSC8503 {
		class GameGridManager;
		class GameStateManager;
		class GameTechRenderer;
		class GameWorld;
		class MenuManager;
		class OptionManager;
		class GameLevel;
		class PhysicsSystem;

		class GameObject;

		class GameGrid;
		class Boss;
		class BossBehaviorTree;
		class PlayerObject;

		class DirectionalLight;

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

			virtual void BossTarget();

			virtual void ProcessState();

			void SetCameraFollow(PlayerObject* p);

			void InitCamera();
			void InitGameExamples();

			void InitDefaultFloor();

			GameObject* AddFloorToWorld(const Vector3& position);

			void AddPlayer(int index, ControllerType type);
			PlayerObject* AddPlayerToWorld(int playerID, const Vector3& position);

			Boss* AddBossToWorld(const Vector3& position, Vector3 dimensions, float inverseMass);
			void BuildLevel();
			void UpdateLevel();

			DebugViewPoint& debugViewPoint;
			GameGridManager& gridManager;
			GameStateManager& gameStateManager;
			GameTechRenderer& renderer;
			GameWorld& gameWorld;
			InputKeyMap& keyMap;
			MenuManager& menuManager;
			OptionManager& optionManager;

			std::unique_ptr<PhysicsSystem> physics;

			int numOfPlayers;
			PlayerObject* players[4];

			DirectionalLight* sunLight = nullptr;

			GameLevel* gameLevel = nullptr;
			Boss* boss = nullptr;

			int playerNum = 0;
			float interval = 0.0f;
		};
	}
}
