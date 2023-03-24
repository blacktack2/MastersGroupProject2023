
/*********************************************************************
 * Author: Xiaoyang Liu
 * Modified: Felix Chiu
 * Date:   February 2023
 *********************************************************************/

#pragma once

#include <fstream>
#include <vector>
#ifdef x64
#include "Vector3.h"
#include "Debug.h"
#include "GameObject.h"
#include "NavigationPath.h"
#include "InkEffectManager.h"
#include "Assets.h"

#endif // x64

#ifdef _ORBIS

#include "../Common/Vector3.h"
#include "../Common/Debug.h"
#include "../Common/GameObject.h"
#include "../Common/NavigationPath.h"
#include "InkEffectManager.h"
#include "../Common/Assets.h"

#endif // _ORBIS

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

			GameNode* NearestNode(Vector3 position);

			void PaintNode(Vector3 position, float radius, paintHell::InkType type);

			void DrawDebugGameGrid();

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

		};

	}
}
