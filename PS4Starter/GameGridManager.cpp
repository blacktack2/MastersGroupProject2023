#include "GameGridManager.h"

using namespace NCL;
using namespace CSC8503;

GameGridManager::GameGridManager() {
}

GameGridManager::~GameGridManager() {
	Clear();
}

void GameGridManager::Update(float dt) {
	for (auto gameGrid : gameGrids) {
		gameGrid->Update(dt);
	}
}
void GameGridManager::AddGameGrid(GameGrid* grid) {
	gameGrids.push_back(grid);
}

GameNode* GameGridManager::NearestNode(Vector3 position) {
	GameNode* node;
	for (auto gameGrid : gameGrids) {
		node = gameGrid->NearestNode(position);
		if (node)
			return node;
	}
	return nullptr;
}

void GameGridManager::PaintPosition(Vector3 position, float paintRadius, paintHell::InkType type) {
	for (auto gameGrid : gameGrids) {
		gameGrid->PaintNode(position, paintRadius, type);
	}
}