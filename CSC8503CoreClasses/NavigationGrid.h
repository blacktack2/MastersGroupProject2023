#pragma once
#include "NavigationMap.h"
#include <string>
namespace NCL {
	namespace CSC8503 {
		enum class GridNodeType {
			DEFAULT,
			WALL,
		};

		struct GridNode {
			GridNode* parent;

			GridNode* connected[4];
			int       costs[4];

			Vector3		position;

			float f;
			float g;

			GridNodeType type;

			GridNode() {
				for (int i = 0; i < 4; ++i) {
					connected[i] = nullptr;
					costs[i] = 0;
				}
				f = 0;
				g = 0;
				type = GridNodeType::DEFAULT;
				parent = nullptr;
			}

			~GridNode() {}
		};

		class NavigationGrid : public NavigationMap	{
		public:
			NavigationGrid(int width = 0, int height = 0, int size = 0, int offsetX = 0, int offsetY = 0);
			NavigationGrid(const std::string&filename);
			~NavigationGrid();

			void AddNode(int x, int y, GridNodeType type);
			void UpdateConnections();

			bool FindPath(const Vector3& from, const Vector3& to, NavigationPath& outPath) override;

			GridNodeType GetAt(const Vector3& position) {
				int px = ((int)position.x / nodeSize) - gridOffsetX;
				int py = ((int)position.y / nodeSize) - gridOffsetY;
				return allNodes[px + py * gridWidth].type;
			}
		protected:
			bool      NodeInList(GridNode* n, std::vector<GridNode*>& list) const;
			GridNode* RemoveBestNode(std::vector<GridNode*>& list) const;
			float     Heuristic(GridNode* hNode, GridNode* endNode) const;
			int nodeSize;
			int gridWidth;
			int gridHeight;
			int gridOffsetX;
			int gridOffsetY;

			GridNode* allNodes;
		};
	}
}

