#include "NavigationGrid.h"

namespace NCL {
	namespace CSC8503 {
		class GameWorld;

		class Maze {
		public:
			Maze(GameWorld& gameWorld, float size, float height, int boundsX, int boundsZ, Vector3 position);

			NavigationGrid& GetNavGrid() {
				return navGrid;
			}

			bool ValidPoint(const Vector3& position) {
				return navGrid.GetAt(position) != GridNodeType::WALL;
			}
		private:
			GameWorld& gameWorld;
			NavigationGrid navGrid;
		};
	}
}