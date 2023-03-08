
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
	//translate world position to grid local position
	Vector3 localPosition = position - gameGridOrigin;

	//translate x,z coord to row col (node)
	int minRow = std::clamp(static_cast<int>(std::round((localPosition.x - radius) / unitLength)), 0, numOfRows - 1);
	int maxRow = std::clamp(static_cast<int>(std::round((localPosition.x + radius) / unitLength)), 0, numOfRows - 1);
	int minCol = std::clamp(static_cast<int>(std::round((localPosition.z - radius) / unitLength)), 0, numOfRows - 1);
	int maxCol = std::clamp(static_cast<int>(std::round((localPosition.z + radius) / unitLength)), 0, numOfRows - 1);

	for (int col = minCol; col < maxCol; col++) {
		for (int row = minRow; row < maxRow; row++) {
			gameNodes[col][row].isInked = true;
			gameNodes[col][row].inkType = type;
		}
	}

}

void GameGrid::DrawDebugGameGrid()
{
	Debug::DrawLine(gameGridOrigin, 
		gameGridOrigin + Vector3(numOfColumns * unitLength * 0.5, 10, numOfRows * unitLength * 0.5), Debug::YELLOW, 0.01f);
	Debug::DrawLine(gameGridOrigin + Vector3(numOfColumns * unitLength , 0.0f , 0.0f), 
		gameGridOrigin + Vector3(numOfColumns * unitLength * 0.5, 10, numOfRows * unitLength * 0.5), Debug::YELLOW, 0.01f);
	Debug::DrawLine(gameGridOrigin + Vector3(0.0f , 0.0f , numOfRows * unitLength), 
		gameGridOrigin + Vector3(numOfColumns * unitLength * 0.5, 10, numOfRows * unitLength * 0.5), Debug::YELLOW, 0.01f);
	Debug::DrawLine(gameGridOrigin + Vector3(numOfColumns * unitLength, 0.0f , numOfRows * unitLength), 
		gameGridOrigin + Vector3(numOfColumns * unitLength * 0.5, 10, numOfRows * unitLength * 0.5), Debug::YELLOW, 0.01f);
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

void GameGrid::PaintNode(Vector3 position, paintHell::InkType type) {
	GameNode* nearestNode = this->NearestNode(position);
	if (nearestNode)
	{
		nearestNode->isInked = true;
		nearestNode->inkType = type;
		Vector3 floatDiff = nearestNode->worldPosition - LastNearestNode.worldPosition;
		float gap = unitLength;
		if (abs(floatDiff.x) > gap || abs(floatDiff.y) > gap || abs(floatDiff.z) > gap) {
			LastNearestNode = *nearestNode;
			TraceNodes.push_back(*nearestNode);
			if (TraceNodes.size() > maxNumOfNodesInTrace) {
				TraceNodes.erase(TraceNodes.begin());
			}
		}
	}
}
