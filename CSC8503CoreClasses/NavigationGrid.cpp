#include "NavigationGrid.h"
#include "Assets.h"

#include <fstream>

using namespace NCL;
using namespace CSC8503;

const int LEFT_NODE		= 0;
const int RIGHT_NODE	= 1;
const int TOP_NODE		= 2;
const int BOTTOM_NODE	= 3;

NavigationGrid::NavigationGrid(int width, int height, int size, int offsetX, int offsetY)	{
	nodeSize	= size;
	gridWidth	= width;
	gridHeight	= height;
	gridOffsetX = offsetX;
	gridOffsetY = offsetY;
	int amount = gridWidth * gridHeight;
	allNodes	= amount == 0 ? nullptr : new GridNode[amount];

	for (int y = 0; y < gridHeight; ++y) {
		for (int x = 0; x < gridWidth; ++x) {
			GridNode&n = allNodes[(gridWidth * y) + x];
			n.type = GridNodeType::DEFAULT;
			n.position = Vector3((float)((x + offsetX) * nodeSize), 0, (float)((y + offsetY) * nodeSize));
		}
	}
}

NavigationGrid::NavigationGrid(const std::string&filename) : NavigationGrid() {
	std::ifstream infile(Assets::DATADIR + filename);

	infile >> nodeSize;
	infile >> gridWidth;
	infile >> gridHeight;
	gridOffsetX = 0;
	gridOffsetY = 0;

	allNodes = new GridNode[gridWidth * gridHeight];

	for (int y = 0; y < gridHeight; ++y) {
		for (int x = 0; x < gridWidth; ++x) {
			GridNode&n = allNodes[(gridWidth * y) + x];
			char type = 0;
			infile >> type;
			n.type = type == '.' ? GridNodeType::DEFAULT : GridNodeType::WALL;
			n.position = Vector3((float)(x * nodeSize), 0, (float)(y * nodeSize));
		}
	}
	
	UpdateConnections();
}

NavigationGrid::~NavigationGrid()	{
	delete[] allNodes;
}

void NavigationGrid::AddNode(int x, int y, GridNodeType type) {
	allNodes[x + y * gridWidth].type = type;
}

void NavigationGrid::UpdateConnections() {
	for (int y = 0; y < gridHeight; ++y) {
		for (int x = 0; x < gridWidth; ++x) {
			GridNode& n = allNodes[(gridWidth * y) + x];		

			if (y > 0) {
				n.connected[0] = &allNodes[(gridWidth * (y - 1)) + x];
			}
			if (y < gridHeight - 1) {
				n.connected[1] = &allNodes[(gridWidth * (y + 1)) + x];
			}
			if (x > 0) {
				n.connected[2] = &allNodes[(gridWidth * (y)) + (x - 1)];
			}
			if (x < gridWidth - 1) {
				n.connected[3] = &allNodes[(gridWidth * (y)) + (x + 1)];
			}
			for (int i = 0; i < 4; ++i) {
				if (n.connected[i]) {
					switch (n.connected[i]->type) {
						case GridNodeType::DEFAULT: n.costs[i]     = 1      ; break;
						case GridNodeType::WALL   : n.connected[i] = nullptr; break;
					}
				}
			}
		}	
	}
	for (int y = 0; y < gridHeight; y++) {
		for (int x = 0; x < gridWidth; x++) {
			std::cout << (allNodes[x + y * gridWidth].type == GridNodeType::DEFAULT ? '.' : '#');
		}
		std::cout << "\n";
	}
}

bool NavigationGrid::FindPath(const Vector3& from, const Vector3& to, NavigationPath& outPath) {
	//need to work out which node 'from' sits in, and 'to' sits in
	int fromX = ((int)from.x / nodeSize) - gridOffsetX;
	int fromZ = ((int)from.z / nodeSize) - gridOffsetY;

	int toX = ((int)to.x / nodeSize) - gridOffsetX;
	int toZ = ((int)to.z / nodeSize) - gridOffsetY;

	if (fromX < 0 || fromX > gridWidth - 1 ||
		fromZ < 0 || fromZ > gridHeight - 1) {
		return false; //outside of map region!
	}

	if (toX < 0 || toX > gridWidth - 1 ||
		toZ < 0 || toZ > gridHeight - 1) {
		return false; //outside of map region!
	}

	GridNode* startNode = &allNodes[(fromZ * gridWidth) + fromX];
	GridNode* endNode	= &allNodes[(toZ * gridWidth) + toX];

	std::vector<GridNode*>  openList;
	std::vector<GridNode*>  closedList;

	openList.push_back(startNode);

	startNode->f = 0;
	startNode->g = 0;
	startNode->parent = nullptr;

	GridNode* currentBestNode = nullptr;

	while (!openList.empty()) {
		currentBestNode = RemoveBestNode(openList);

		if (currentBestNode == endNode) {			//we've found the path!
			GridNode* node = endNode;
			while (node != nullptr) {
				outPath.PushWaypoint(node->position);
				node = node->parent;
			}
			return true;
		}
		else {
			for (int i = 0; i < 4; ++i) {
				GridNode* neighbour = currentBestNode->connected[i];
				if (!neighbour) { //might not be connected...
					continue;
				}	
				bool inClosed	= NodeInList(neighbour, closedList);
				if (inClosed) {
					continue; //already discarded this neighbour...
				}

				float h = Heuristic(neighbour, endNode);				
				float g = currentBestNode->g + currentBestNode->costs[i];
				float f = h + g;

				bool inOpen		= NodeInList(neighbour, openList);

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

bool NavigationGrid::NodeInList(GridNode* n, std::vector<GridNode*>& list) const {
	std::vector<GridNode*>::iterator i = std::find(list.begin(), list.end(), n);
	return i == list.end() ? false : true;
}

GridNode*  NavigationGrid::RemoveBestNode(std::vector<GridNode*>& list) const {
	std::vector<GridNode*>::iterator bestI = list.begin();

	GridNode* bestNode = *list.begin();

	for (auto i = list.begin(); i != list.end(); ++i) {
		if ((*i)->f < bestNode->f) {
			bestNode	= (*i);
			bestI		= i;
		}
	}
	list.erase(bestI);

	return bestNode;
}

float NavigationGrid::Heuristic(GridNode* hNode, GridNode* endNode) const {
	return (hNode->position - endNode->position).Length();
}