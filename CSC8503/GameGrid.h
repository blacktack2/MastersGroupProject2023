
/*********************************************************************
 * Author: Xiaoyang Liu
 * Modified: Felix Chiu
 * Date:   February 2023
 *********************************************************************/

#pragma once

#include <fstream>
#include <vector>
#include "Vector3.h"
#include "Debug.h"
#include "GameObject.h"
#include "NavigationPath.h"
#include "InkEffectManager.h"
#include "Assets.h"



using namespace NCL::Maths;

namespace NCL
{
	namespace CSC8503	// TODO: change the name of the namespace
	{

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

			~GameNode() {	}		// uncomment when the contents of the destructor should not be empty

			int row;
			int column;
			Vector3 worldPosition;

			bool isInked;
			paintHell::InkType inkType = paintHell::InkType::NoInk;
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
			GameGrid(Vector3 gridOrigin, float totalLength, float totalWidth, float unitLength);
			~GameGrid();
			void Update(float dt) {
				DrawDebugGameGrid();
			}

			//void UpdateHealingKits(GameObject* obj, Vector3 dimension)
			//{
			//	float rx = dimension.x;
			//	float ry = dimension.y;
			//	float rz = dimension.z;

			//	for (int i = 0; i < healingKits.size(); i++)
			//	{
			//		Vector3 radius = obj->GetTransform().GetScale() / 2;
			//		if ((healingKits[i]->GetTransform().GetGlobalPosition() - obj->GetTransform().GetGlobalPosition()).Length() < radius.x + rx ||
			//			(healingKits[i]->GetTransform().GetGlobalPosition() - obj->GetTransform().GetGlobalPosition()).Length() < radius.y + ry ||
			//			(healingKits[i]->GetTransform().GetGlobalPosition() - obj->GetTransform().GetGlobalPosition()).Length() < radius.z + rz)
			//		{
			//			healingKits[i]->Delete();
			//			healingKits.erase(healingKits.begin() + i);
			//			// TODO: update obj's health
			//		}
			//	}
			//}


			GameNode* NearestNode(Vector3 position);

			void PaintNode(Vector3 position, float radius, paintHell::InkType type);

			void DrawDebugGameGrid();

			void PaintNode(Vector3 position, paintHell::InkType type);

			std::vector<GameNode> GetTraceNodes();

			GameNode* GetRandomNode();

			//healing stuff (shouldn't be here)
			float GetHealingKitTimer();

			//Vector3 GetRandomLocationToAddHealingKit(HealingKit* obj)
			//{
			//	healingKitTimer = 0.0f;

			//	healingKits.push_back(obj);

			//	GameNode* node = GetRandomNode();
			//	while (node->type == Impassable)	// this algorithm would cause a longer time if most nodes are Impassable
			//	{
			//		node = GetRandomNode();
			//	}
			//	return node->worldPosition;
			//}

			/*std::vector<HealingKit*> GetHealingKits()
			{
				return healingKits;
			}*/

			std::vector<std::vector<GameNode>> GetAllNodes()
			{
				return gameNodes;
			}

		protected:

			int numOfRows = 0;
			int numOfColumns = 0;
			float unitLength;

			std::vector<std::vector<GameNode>> gameNodes;
			Vector3 gameGridOrigin;

			float healingKitTimer = 0.0f;
			//std::vector<HealingKit*> healingKits;

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
