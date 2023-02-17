
/*********************************************************************
 * Author: Xiaoyang Liu
 * Date:   February 2023
 *********************************************************************/

#pragma once

#include <vector>
#include "Vector3.h"
#include "Debug.h"
#include "GameObject.h"
#include "NavigationPath.h"
 //#include "Boss.h"
 //#include "PlayerObject.h"

using namespace NCL::Maths;

namespace NCL
{
	namespace CSC8503	// TODO: change the name of the namespace
	{
		enum InkType
		{
			NoInk,			// No effect
			Toxic,			// causing damage
			Medicative,		// results in slower speed but can heal health
			Sad,			// feeling sad when stepping on, results in less damage but higher defense
			Mad,			// feeling angry when stepping on, results in higher damage but less defense
			Happy			// feeling happy when stepping on, results in higher jumping and faster speed
		};

		enum NodeType
		{
			Air,
			Impassable,
			Ink
		};

		class HealingKit : public GameObject
		{

		public:
			HealingKit()
				: GameObject()
			{  }

			//~HealingKit(){}

			virtual void Update(float dt) override
			{
				deltaTime = dt;

				if (this->GetTransform().GetGlobalPosition().y < 1.0f)		// please fix the physics system
				{
					GetTransform().SetPosition({ GetTransform().GetGlobalPosition().x, 1.0f, GetTransform().GetGlobalPosition().z });
				}
			}

		protected:

			float deltaTime = 0.0f;
			float healthTOHeal = 33.3f;
		};

		struct GameNode
		{
			GameNode(int r, int c, Vector3 wPos, bool inked = false)
			{
				row = r;
				column = c;
				worldPosition = wPos;
				isInked = inked;

				for (int i = 0; i < 8; ++i) {
					connected[i] = nullptr;
					costs[i] = 0;
				}
				f = 0;
				g = 0;
				type = Air;			// TODO: modify this type at a latter point
				parent = nullptr;
			}

			//~GameNode() {	}		// uncomment when the contents of the destructor should not be empty

			int row;
			int column;
			Vector3 worldPosition;

			bool isInked;
			InkType inkType = NoInk;
			NodeType type;				// the type of object represented by the node

			GameNode* parent;
			GameNode* connected[8];
			float		  costs[8];
			float f;
			float g;

		};

		class GameGrid
		{
		public:
			GameGrid(Vector3 gridOrigin, int rows, int columns, float totalLength, float totalWidth)
			{
				numOfRows = rows;
				numOfColumns = columns;

				gameGridOrigin = gridOrigin;
				float rowUnitLength = totalLength / (rows - 1);
				float columnUnitLength = totalWidth / (columns - 1);
				float width = gridOrigin.z;
				for (int row = 0; row < rows; row++)
				{
					std::vector<GameNode> r;
					float length = gridOrigin.x;
					for (int column = 0; column < columns; column++)
					{
						GameNode n = GameNode(row + 1, column + 1, Vector3{ length, gridOrigin.y, width });

						r.push_back(n);
						length += columnUnitLength;
					}
					gameNodes.push_back(r);
					width += rowUnitLength;
				}
				UpdateGrid();
			}

			void UpdateHealingKits(GameObject* obj, Vector3 dimension)
			{
				float rx = dimension.x;
				float ry = dimension.y;
				float rz = dimension.z;

				for (int i = 0; i < healingKits.size(); i++)
				{
					Vector3 radius = obj->GetTransform().GetScale() / 2;
					if ((healingKits[i]->GetTransform().GetGlobalPosition() - obj->GetTransform().GetGlobalPosition()).Length() < radius.x + rx ||
						(healingKits[i]->GetTransform().GetGlobalPosition() - obj->GetTransform().GetGlobalPosition()).Length() < radius.y + ry ||
						(healingKits[i]->GetTransform().GetGlobalPosition() - obj->GetTransform().GetGlobalPosition()).Length() < radius.z + rz)
					{
						healingKits[i]->Delete();
						healingKits.erase(healingKits.begin() + i);
						// TODO: update obj's health
					}
				}
			}

			void UpdateGrid(float dt = 0.0f)
			{
				healingKitTimer += dt;

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

			bool FindCatmullRomPath(const Vector3& from, const Vector3& to, NavigationPath& outPath)
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

			GameNode* NearestNode(Vector3 position)
			{
				float closestDistance = 99.0f;		// return null pointer if the closest node is further away than this distance
				GameNode* resultingNode = nullptr;
				for (auto& currentRow : gameNodes)
				{
					for (auto& currentNode : currentRow)
					{
						float currentDistance = (position - currentNode.worldPosition).Length();
						if (currentDistance < closestDistance)
						{
							closestDistance = currentDistance;
							resultingNode = &currentNode;
						}
					}
				}
				return resultingNode;
			}

			void DrawGameGrid()
			{

				for (auto& currentRow : gameNodes)
				{
					for (auto& currentNode : currentRow)
					{
						if (currentNode.isInked)
						{
							Debug::DrawLine(currentNode.worldPosition, currentNode.worldPosition + Vector3{ 0,2,0 }, Vector4(1, 0, 0, 1));
						}
						else
						{
							Debug::DrawLine(currentNode.worldPosition, currentNode.worldPosition + Vector3{ 0,2,0 }, Vector4(0, 0, 1, 1));
						}
					}
				}
			}

			void UpdateTrace(GameObject* obj)
			{
				GameNode* nearestNode = this->NearestNode(obj->GetTransform().GetGlobalPosition());
				if (nearestNode)
				{
					nearestNode->isInked = true;
					Vector3 floatDiff = nearestNode->worldPosition - LastNearestNode.worldPosition;
					float gap = 2.0f;
					if (abs(floatDiff.x) > gap || abs(floatDiff.y) > gap || abs(floatDiff.z) > gap) {
						LastNearestNode = *nearestNode;
						TraceNodes.push_back(*nearestNode);
						if (TraceNodes.size() > maxNumOfNodesInTrace) {
							TraceNodes.erase(TraceNodes.begin());
						}
					}
				}
				//this->DrawGameGrid();
			}

			std::vector<GameNode> GetTraceNodes()
			{
				return TraceNodes;
			}

			GameNode* GetRandomNode()
			{
				// Note that RAND_MAX is at least 32767
				int r = (std::rand() % numOfRows);
				int c = (std::rand() % numOfColumns);

				return &(gameNodes[r][c]);
			}

			float GetHealingKitTimer()
			{
				return healingKitTimer;
			}

			Vector3 GetRandomLocationToAddHealingKit(HealingKit* obj)
			{
				healingKitTimer = 0.0f;

				healingKits.push_back(obj);

				GameNode* node = GetRandomNode();
				while (node->type == Impassable)	// this algorithm would cause a longer time if most nodes are Impassable
				{
					node = GetRandomNode();
				}
				return node->worldPosition;
			}

			std::vector<HealingKit*> GetHealingKits()
			{
				return healingKits;
			}

		protected:

			int numOfRows = 0;
			int numOfColumns = 0;

			std::vector<std::vector<GameNode>> gameNodes;
			Vector3 gameGridOrigin;

			float healingKitTimer = 0.0f;
			std::vector<HealingKit*> healingKits;

			//Boss* boss = nullptr;
			//PlayerObject* player = nullptr;

			// For leaving a trace of ink on the floor:
			std::vector<GameNode> TraceNodes;
			const int maxNumOfNodesInTrace = 100;
			GameNode LastNearestNode{ -1,-1,{99999,99999,99999} };

			// For pathfinding:
			bool NodeInList(GameNode* n, std::vector<GameNode*>& list) const {
				std::vector<GameNode*>::iterator i = std::find(list.begin(), list.end(), n);
				return i == list.end() ? false : true;
			}

			GameNode* RemoveBestNode(std::vector<GameNode*>& list) const {
				std::vector<GameNode*>::iterator bestI = list.begin();
				GameNode* bestNode = *list.begin();

				for (auto i = list.begin(); i != list.end(); ++i) {
					if ((*i)->f < bestNode->f) {
						bestNode = (*i);
						bestI = i;
					}
				}
				list.erase(bestI);

				return bestNode;
			}

			float Heuristic(GameNode* hNode, GameNode* endNode) const {
				return (hNode->worldPosition - endNode->worldPosition).Length();		// Manhattan distance
			}
		};

	}
}
