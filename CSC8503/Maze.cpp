#include "Maze.h"

#include "AssetLibrary.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "PhysicsObject.h"
#include "RenderObject.h"

using namespace NCL;
using namespace CSC8503;

Maze::Maze(GameWorld& gameWorld, float size, float height, int boundsX, int boundsZ, Vector3 position)
: gameWorld(gameWorld), navGrid(boundsX, boundsZ, size, (int)(position.x / size - boundsX * 0.5f), (int)(position.z / size - boundsZ * 0.5f)) {
	Vector3 cellSize = Vector3(size, height, size);

	int minX = boundsX * 0.5f;
	int maxX = boundsX - minX;
	int minZ = boundsZ * 0.5f;
	int maxZ = boundsZ - minZ;
	for (int z = -minZ; z < maxZ; z++) {
		for (int x = -minX; x < maxX; x++) {
			if (rand() % 3 == 0) {
				GameObject* cell = new GameObject(gameWorld, std::string("MazeCell[").append(std::to_string(x)).append(",").append(std::to_string(z)).append("]"));
				cell->SetBoundingVolume((CollisionVolume*)new AABBVolume(cellSize * 0.5f));
				cell->SetPhysicsObject(new PhysicsObject(&cell->GetTransform(), cell->GetBoundingVolume()));
				cell->SetRenderObject(new RenderObject(&cell->GetTransform(), AssetLibrary::GetMesh("cube"), AssetLibrary::GetTexture("basic"), AssetLibrary::GetShader("basic")));
				cell->GetPhysicsObject()->SetStatic();
				cell->GetPhysicsObject()->SetInverseMass(0);
				cell->GetRenderObject()->SetColour(Vector4(0, 0.6f, 0, 1));
				cell->GetTransform().SetPosition(position + cellSize * Vector3(x, 0.5f, z))
					                .SetScale(cellSize);
				gameWorld.AddGameObject(cell);

				navGrid.AddNode(minX + x, minZ + z, GridNodeType::WALL);
			} else {
				navGrid.AddNode(minX + x, minZ + z, GridNodeType::DEFAULT);
			}
		}
	}
	navGrid.UpdateConnections();
}
