
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

	UpdateGrid();
}
GameGrid::~GameGrid() {

}
void GameGrid::UpdateGrid(float dt)
{
	for (int y = 0; y < gameNodes.size(); y++)
	{
		for (int x = 0; x < gameNodes[y].size(); x++)
		{
			GameNode& n = gameNodes[y][x];

			if (y > 0)
			{ //get the above node
				n.connected[0] = &gameNodes[y - 1][x];
			}
			if (y < gameNodes.size() - 1)
			{ //get the below node
				n.connected[1] = &gameNodes[y + 1][x];
			}
			if (x > 0)
			{ //get left node
				n.connected[2] = &gameNodes[y][x - 1];
			}
			if (x < gameNodes[y].size() - 1)
			{ //get right node
				n.connected[3] = &gameNodes[y][x + 1];
			}
			// 4 diagonal nodes:
			if (y > 0 && x > 0)
			{ //get the top-left node
				n.connected[4] = &gameNodes[y - 1][x - 1];
			}
			if (y < gameNodes.size() - 1 && x > 0)
			{ //get the bottom-left node
				n.connected[5] = &gameNodes[y + 1][x - 1];
			}
			if (y > 0 && x < gameNodes[y].size() - 1)
			{ //get top-right node
				n.connected[6] = &gameNodes[y - 1][x + 1];
			}
			if (y < gameNodes.size() - 1 && x < gameNodes[y].size() - 1)
			{ //get bottom-right node
				n.connected[7] = &gameNodes[y + 1][x + 1];
			}

			for (int i = 0; i < 8; ++i)
			{
				if (n.connected[i]) {
					if (n.connected[i]->type == Air)
					{
						if (i >= 4)
						{
							n.costs[i] = 1.414f;	// cost more on diagonal
						}
						else
						{
							n.costs[i] = 1;
						}
					}
					if (n.connected[i]->type == Ink && false)	// delete "&& false" after filling in the body
					{
						// TODO:
						if (n.connected[i]->inkType == 0)
						{
							// ...
						}
						// ...
					}
					if (n.connected[i]->type == Impassable)
					{
						n.connected[i] = nullptr; // You may not pass! disconnect!
					}
				}
			}
		}
	}
}

bool GameGrid::FindCatmullRomPath(const Vector3& from, const Vector3& to, NavigationPath& outPath)
{
	//need to work out which node 'from' sits in, and 'to' sits in

	GameNode* startNode = NearestNode(from);
	GameNode* endNode = NearestNode(to);
	if (startNode == nullptr || endNode == nullptr)
	{
		return false;	// starting point too far away from the grid
	}

	std::vector<GameNode*>  openList;
	std::vector<GameNode*>  closedList;

	openList.push_back(startNode);

	startNode->f = 0;
	startNode->g = 0;
	startNode->parent = nullptr;

	GameNode* currentBestNode = nullptr;

	while (!openList.empty()) {
		currentBestNode = RemoveBestNode(openList);

		if (currentBestNode == endNode) {			//we've found the path!
			GameNode* node = endNode;

			bool useCatrullRom = true;

			if (!useCatrullRom) {
				while (node != nullptr) {
					outPath.PushWaypoint(node->worldPosition);
					node = node->parent;
				}
			}
			else {
				std::vector<Vector3> points;
				std::vector<Vector3> samples;

				while (node != nullptr) {
					points.emplace_back(node->worldPosition);
					node = node->parent;
				}
				if (points.size() == 1) {
					outPath.PushWaypoint(points[0]);
				}
				else if (points.size() != 0) {
					points.insert(points.begin(), points[points.size() - 1]);	// form a closed loop, to make the path more curved

					points.push_back(points[1]);

					// we have at least 4 points at this stage (so that it suffices to construct a segment of the Catmull-Rom spline)
					for (int i = 1; i < points.size() - 2; i++) {
						Vector3 p0 = points[i - 1];
						Vector3 p1 = points[i];
						Vector3 p2 = points[i + 1];
						Vector3 p3 = points[i + 2];

						float t01 = std::pow((p1 - p0).Length(), 0.5);	// 0.5 corresponding to "centripetal" Catmull-Rom spline
						float t12 = std::pow((p2 - p1).Length(), 0.5);
						float t23 = std::pow((p3 - p2).Length(), 0.5);

						float tension = 0.0f;							// 0 tension gives a more curved path
						Vector3 m1 = ((p2 - p1) + (((p1 - p0) / t01) - ((p2 - p0) / (t01 + t12))) * t12) * (1.0f - tension);
						Vector3 m2 = ((p2 - p1) + (((p3 - p2) / t23) - ((p3 - p1) / (t12 + t23))) * t12) * (1.0f - tension);

						// The coefficients are:
						Vector3 a = ((p1 - p2) * 2.0f) + m1 + m2;;
						Vector3 b = ((p1 - p2) * -3.0f) - m1 - m1 - m2;
						Vector3 c = m1;
						Vector3 d = p1;

						int samplingRate = 100;		// decrease sampling rate if it's causing performance issue

						for (int i = 0; i < samplingRate + 1; i++) {
							float t = i * (1.0f / samplingRate);
							Vector3 s = a * t * t * t + b * t * t + c * t + d;	// the sampled point
							samples.push_back(s);
						}
					}
					// Now we have all the sample points on the path (here I ignore the points that are being double-counted)
					for (const auto& v : samples) {
						outPath.PushWaypoint(v);
					}
				}
			}

			return true;
		}
		else {
			for (int i = 0; i < 8; ++i) {		//////// 4 changed to 8
				GameNode* neighbour = currentBestNode->connected[i];
				if (!neighbour) { //might not be connected...
					continue;
				}
				bool inClosed = NodeInList(neighbour, closedList);
				if (inClosed) {
					continue; //already discarded this neighbour...
				}

				float h = Heuristic(neighbour, endNode);
				float g = currentBestNode->g + currentBestNode->costs[i];
				float f = h + g;

				bool inOpen = NodeInList(neighbour, openList);

				if (!inOpen) { //first time we've seen this neighbour
					openList.emplace_back(neighbour);
				}
				if (!inOpen || f < neighbour->f) {//might be a better route to this neighbour
					neighbour->parent = currentBestNode;
					neighbour->f = f;
					neighbour->g = g;
				}
			}
			closedList.emplace_back(currentBestNode);
		}
	}
	return false; //open list emptied out with no path!
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

std::vector<GameNode*> GameGrid::AreaNode(Vector3 position, float radius)
{
	//translate world position to grid local position
	Vector3 localPosition = position - gameGridOrigin;

	//translate x,z coord to row col (node)
	int minRow = std::round((localPosition.x - radius) / unitLength);
	int maxRow = std::round((localPosition.x + radius) / unitLength);
	int minCol = std::round((localPosition.z - radius) / unitLength);
	int maxCol = std::round((localPosition.z + radius) / unitLength);
	




	std::vector<GameNode*> t;
	return t;

}

void GameGrid::DrawDebugGameGrid()
{

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

std::vector<GameNode> GameGrid::GetTraceNodes()
{
	return TraceNodes;
}

GameNode* GameGrid::GetRandomNode()
{
	// Note that RAND_MAX is at least 32767
	int r = (std::rand() % numOfRows);
	int c = (std::rand() % numOfColumns);

	return &(gameNodes[r][c]);
}

float GameGrid::GetHealingKitTimer()
{
	return healingKitTimer;
}