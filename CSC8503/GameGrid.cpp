
/*********************************************************************
 * Author: Xiaoyang Liu
 * Modified: Felix Chiu
 * Date:   February 2023
 *********************************************************************/

#pragma once

#include "GameGrid.h"

using namespace NCL;
using namespace CSC8503;

GameGrid::GameGrid(Vector3 gridOrigin, float totalLength, float totalWidth, float unitLength)
{
	numOfRows = (int)(totalLength / unitLength);
	numOfColumns = (int)(totalWidth / unitLength);
	this->unitLength = unitLength;

	gameGridOrigin = gridOrigin - Vector3(totalLength * 0.5, 0, totalWidth * 0.5);
	float width = gameGridOrigin.z;
	for (int row = 0; row < numOfRows; row++)
	{
		std::vector<GameNode> r;
		float length = gameGridOrigin.x;
		for (int column = 0; column < numOfColumns; column++)
		{
			GameNode n = GameNode(row + 1, column + 1, Vector3{ length, gameGridOrigin.y, width });

			r.push_back(n);
			length += unitLength;
		}
		gameNodes.push_back(r);
		width += unitLength;
	}
}
GameGrid::~GameGrid() {

}

GameNode* GameGrid::NearestNode(Vector3 position)
{
	//translate world position to grid local position
	Vector3 localPosition = position - gameGridOrigin;

	//translate x,z coord to row col (node)
	int row = std::round(localPosition.x / unitLength);
	int col = std::round(localPosition.z / unitLength);
	if (row < 0 || row >= numOfRows) {
		return nullptr;
	}
	if (col < 0 || col >= numOfColumns) {
		return nullptr;
	}

	GameNode* resultingNode = &gameNodes[col][row];

	//modify node
	return resultingNode;
}

void GameGrid::PaintNode(Vector3 position, float radius, paintHell::InkType type) {
	if (position.y > 8) {
		return;
	}
	//translate world position to grid local position
	Vector3 localPosition = position - gameGridOrigin;
	/*
	Debug::DrawLine(position, position + Vector3{ 0, 1, 0 }, Debug::CYAN, 1.0f);
	*/
	float halfLength = unitLength / 2;
	float searchRadius = radius + 0.5f;

	//translate x,z coord to row col (node)
	int minRow = std::clamp(static_cast<int>(std::round((localPosition.x - searchRadius) / unitLength) + halfLength), 0, numOfRows);
	int maxRow = std::clamp(static_cast<int>(std::round((localPosition.x + searchRadius) / unitLength) + halfLength), 0, numOfRows);
	int minCol = std::clamp(static_cast<int>(std::round((localPosition.z - searchRadius) / unitLength) + halfLength), 0, numOfRows);
	int maxCol = std::clamp(static_cast<int>(std::round((localPosition.z + searchRadius) / unitLength) + halfLength), 0, numOfRows);
	GameNode* node;
	for (int col = minCol; col < maxCol; col++) {
		for (int row = minRow; row < maxRow; row++) {
			node = &gameNodes[col][row];
			if (abs((node->worldPosition - position).Length()) < radius) {
				gameNodes[col][row].isInked = true;
				gameNodes[col][row].inkType = type;
			}
			
		}
	}
	/*
	Debug::DrawLine(gameNodes[minCol][minRow].worldPosition, gameNodes[minCol][minRow].worldPosition + Vector3{ 0, 1, 0 }, Debug::YELLOW, 1.0f);
	Debug::DrawLine(gameNodes[minCol][maxRow].worldPosition, gameNodes[minCol][maxRow].worldPosition + Vector3{ 0, 1, 0 }, Debug::YELLOW, 1.0f);
	Debug::DrawLine(gameNodes[maxCol][minRow].worldPosition, gameNodes[maxCol][minRow].worldPosition + Vector3{ 0, 1, 0 }, Debug::YELLOW, 1.0f);
	Debug::DrawLine(gameNodes[maxCol][maxRow].worldPosition, gameNodes[maxCol][maxRow].worldPosition + Vector3{ 0, 1, 0 }, Debug::YELLOW, 1.0f);
	*/
}

void GameGrid::DrawDebugGameGrid()
{
	/*
	Debug::DrawLine(gameGridOrigin, 
		gameGridOrigin + Vector3(numOfColumns * unitLength * 0.5, 10, numOfRows * unitLength * 0.5), Debug::YELLOW, 0.01f);
	Debug::DrawLine(gameGridOrigin + Vector3(numOfColumns * unitLength , 0.0f , 0.0f), 
		gameGridOrigin + Vector3(numOfColumns * unitLength * 0.5, 10, numOfRows * unitLength * 0.5), Debug::YELLOW, 0.01f);
	Debug::DrawLine(gameGridOrigin + Vector3(0.0f , 0.0f , numOfRows * unitLength), 
		gameGridOrigin + Vector3(numOfColumns * unitLength * 0.5, 10, numOfRows * unitLength * 0.5), Debug::YELLOW, 0.01f);
	Debug::DrawLine(gameGridOrigin + Vector3(numOfColumns * unitLength, 0.0f , numOfRows * unitLength), 
		gameGridOrigin + Vector3(numOfColumns * unitLength * 0.5, 10, numOfRows * unitLength * 0.5), Debug::YELLOW, 0.01f);
	*/
	for (auto& currentRow : gameNodes)
	{
		for (auto& currentNode : currentRow)
		{
			switch (currentNode.inkType)
			{
			case paintHell::InkType::BossDamage:
				Debug::DrawLine(currentNode.worldPosition, currentNode.worldPosition + Vector3{ 0, 0.5, 0 }, Vector4(0, 1, 0.2, 1), 0.01f);
				break;
			case paintHell::InkType::PlayerDamage:
				Debug::DrawLine(currentNode.worldPosition, currentNode.worldPosition + Vector3{ 0, 0.5, 0 }, Vector4(1, 0, 0, 1), 0.01f);
				break;
			default:
				break;
			}
		}
	}
}
