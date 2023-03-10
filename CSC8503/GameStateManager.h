#pragma once
enum class GameState {
	Lobby,
	Start,
	OnGoing,
	Win,
	Lose,
	Quit,
	Invalid
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
