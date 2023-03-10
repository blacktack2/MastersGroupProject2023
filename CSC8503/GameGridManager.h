#pragma once
#include "GameGrid.h"

namespace NCL {
	namespace CSC8503 {
		class GameGridManager {
		public:
			static GameGridManager& instance() {
				static GameGridManager INSTANCE;
				return INSTANCE;
			}
			void Update(float dt);

			void Clear() {
				for (auto gameGrid : gameGrids) {
					delete gameGrid;
				}
				gameGrids.clear();
			}

			void AddGameGrid(GameGrid* grid);

			GameNode* NearestNode(Vector3 position);

			void PaintPosition(Vector3 position, float paintRadius, NCL::InkType type);
		private:
			GameGridManager();
			~GameGridManager();

			std::vector<GameGrid*> gameGrids;
		};
	}
}
