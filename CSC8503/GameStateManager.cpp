#include "GameStateManager.h"
#include "InputKeyMap.h"
#include "Debug.h"

using namespace NCL;
using namespace CSC8503;

GameStateManager::GameStateManager(): gameState(GameState::OnGoing) {
}

GameStateManager::~GameStateManager() {
}

void GameStateManager::Update(float dt) {
	switch (gameState) {
	case GameState::Win:
		//Debug::Print("You Win!", Vector2(5, 80), Vector4(0, 1, 0, 1));
		if(isNetworked)
			Debug::Print("Press [Space] or [Start] to return to lobby", Vector2(5, 90), Vector4(1, 1, 1, 1));
		else
			Debug::Print("Press [Space] or [Start] to play again", Vector2(5, 90), Vector4(1, 1, 1, 1));
		break;
	case GameState::Lose:
		//Debug::Print("You got Inked!", Vector2(5, 80), Vector4(1, 0, 0, 1));
		if (isNetworked)
			Debug::Print("Press [Space] or [Start] to return to lobby", Vector2(5, 90), Vector4(1, 1, 1, 1));
		else
			Debug::Print("Press [Space] or [Start] to play again", Vector2(5, 90), Vector4(1, 1, 1, 1));
		break;
	}
}
