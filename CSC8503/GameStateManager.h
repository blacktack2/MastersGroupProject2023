#pragma once

enum class GameState {
	Main,
	Option,
	OnGoing,
	Paused,
	Win,
	Lose,
	Quit,
};

namespace NCL {
	namespace CSC8503 {
		class GameStateManager {
		private:
			GameStateManager();
			~GameStateManager();
			GameState gameState;
		public:
			static GameStateManager& instance() {
				static GameStateManager INSTANCE;
				return INSTANCE;
			}
			void Update(float dt);

			GameState GetGameState() {
				return gameState;
			};
			void SetGameState(GameState gameState) {
				this->gameState = gameState;
			};
		};
	}
}
