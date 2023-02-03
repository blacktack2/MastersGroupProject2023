#include "Camera.h"
#include "CollisionDetection.h"
#include "Constraint.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "PhysicsObject.h"

using namespace NCL;
using namespace NCL::CSC8503;

GameWorld::GameWorld() : staticQuadTree(Vector2(1024, 1024), 7, 6), dynamicQuadTree(Vector2(1024, 1024), 7, 6) {
	mainCamera = new Camera();

	shuffleConstraints	= false;
	shuffleObjects		= false;
	worldIDCounter		= 0;
	worldStateCounter	= 0;
}

GameWorld::~GameWorld()	{
}

void GameWorld::Clear() {
	dynamicQuadTree.Clear();
	staticQuadTree.Clear();
	gameObjects.clear();
	constraints.clear();
	worldIDCounter		= 0;
	worldStateCounter	= 0;
}

void GameWorld::ClearAndErase() {
	for (auto& i : gameObjects) {
		delete i;
	}
	for (auto& i : constraints) {
		delete i;
	}
	Clear();
}

void GameWorld::AddGameObject(GameObject* o) {
	gameObjects.emplace_back(o);
	o->SetWorldID(worldIDCounter++);
	worldStateCounter++;
}

void GameWorld::RemoveGameObject(GameObject* o, bool andDelete) {
	gameObjects.erase(std::remove(gameObjects.begin(), gameObjects.end(), o), gameObjects.end());
	if (andDelete) {
		delete o;
	}
	worldStateCounter++;
}

void GameWorld::GetObjectIterators(
	GameObjectIterator& first,
	GameObjectIterator& last) const {

	first	= gameObjects.begin();
	last	= gameObjects.end();
}

void GameWorld::OperateOnContents(GameObjectFunc f) {
	for (GameObject* g : gameObjects) {
		f(g);
	}
}

void GameWorld::PreUpdateWorld() {
	gameObjects.erase(std::remove_if(gameObjects.begin(), gameObjects.end(),
		[](GameObject* o) {
			if (o->IsMarkedDelete()) {
				delete o;
				return true;
			}
			return false;
		}
	), gameObjects.end());
	UpdateDynamicTree();
}

void GameWorld::UpdateWorld(float dt) {
	runTime += dt;

	auto rng = std::default_random_engine{};

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine e(seed);

	if (shuffleObjects) {
		std::shuffle(gameObjects.begin(), gameObjects.end(), e);
	}

	if (shuffleConstraints) {
		std::shuffle(constraints.begin(), constraints.end(), e);
	}

	for (int i = 0; i < gameObjects.size(); i++) {
		gameObjects[i]->Update(dt);
	}

	//dynamicQuadTree.DebugDraw();
	//staticQuadTree.DebugDraw();
}

void GameWorld::PostUpdateWorld() {
}

void GameWorld::UpdateStaticTree() {
	staticQuadTree.Clear();
	OperateOnContents(
		[](GameObject* g) {
			g->UpdateBroadphaseAABB();
		}
	);

	for (auto i = gameObjects.begin(); i != gameObjects.end(); i++) {
		if ((*i)->GetPhysicsObject() && (*i)->GetPhysicsObject()->IsStatic()) {
			Vector3 halfSizes;
			if (!(*i)->GetBroadphaseAABB(halfSizes)) {
				continue;
			}
			Vector3 pos = (*i)->GetTransform().GetGlobalPosition();
			staticQuadTree.Insert(*i, Vector2(pos.x, pos.z), Vector2(halfSizes.x, halfSizes.z));
		}
	}
}

void GameWorld::UpdateDynamicTree() {
	dynamicQuadTree.Clear();

	for (auto i = gameObjects.begin(); i != gameObjects.end(); i++) {
		if ((*i)->GetPhysicsObject() && !(*i)->GetPhysicsObject()->IsStatic()) {
			Vector3 halfSizes;
			if (!(*i)->GetBroadphaseAABB(halfSizes)) {
				continue;
			}
			Vector3 pos = (*i)->GetTransform().GetGlobalPosition();
			dynamicQuadTree.Insert(*i, Vector2(pos.x, pos.z), Vector2(halfSizes.x, halfSizes.z));
		}
	}
}

void GameWorld::OperateOnStaticTree(QuadTreeNode::QuadTreeFunc func, const Vector2* subsetPos, const Vector2* subsetSize) {
	if (subsetPos == nullptr || subsetSize == nullptr) {
		staticQuadTree.OperateOnContents(func);
	} else {
		staticQuadTree.OperateOnContents(func, *subsetPos, *subsetSize);
	}
}

void GameWorld::OperateOnDynamicTree(QuadTreeNode::QuadTreeFunc func, const Vector2* subsetPos, const Vector2* subsetSize) {
	if (subsetPos == nullptr || subsetSize == nullptr) {
		dynamicQuadTree.OperateOnContents(func);
	} else {
		dynamicQuadTree.OperateOnContents(func, *subsetPos, *subsetSize);
	}
}

bool GameWorld::Raycast(Ray& r, RayCollision& closestCollision, bool closestObject, GameObject* ignoreThis) {
	//The simplest raycast just goes through each object and sees if there's a collision
	RayCollision collision;

	dynamicQuadTree.OperateOnContents(
		[&](std::list<QuadTreeEntry>& data, const Vector2& staticPos, const Vector2& staticSize) {
			for (auto i : data) {
				GameObject* go = i.object;
				if (!go->GetBoundingVolume() || go == ignoreThis) {
					continue;
				}
				RayCollision newCollision;
				if (CollisionDetection::RayIntersection(r, *go, newCollision)) {
					if (!closestObject) {
						closestCollision = newCollision;
						closestCollision.node = go;
						return;
					} else if (newCollision.rayDistance < collision.rayDistance) {
						newCollision.node = go;
						collision = newCollision;
					}
				}
			}
		}, r
	);

	if (closestObject || collision.node == nullptr) {
		staticQuadTree.OperateOnContents(
			[&](std::list<QuadTreeEntry>& data, const Vector2& staticPos, const Vector2& staticSize) {
				for (auto i : data) {
					GameObject* go = i.object;
					if (!go->GetBoundingVolume() || go == ignoreThis) {
						continue;
					}
					RayCollision newCollision;
					if (CollisionDetection::RayIntersection(r, *go, newCollision)) {
						if (!closestObject) {
							closestCollision = newCollision;
							closestCollision.node = go;
							return;
						} else if (newCollision.rayDistance < collision.rayDistance) {
							newCollision.node = go;
							collision = newCollision;
						}
					}
				}
			}, r
		);
	}

	if (collision.node) {
		closestCollision      = collision;
		closestCollision.node = collision.node;
		return true;
	}
	return false;
}


/*
Constraint Tutorial Stuff
*/

void GameWorld::AddConstraint(Constraint* c) {
	constraints.emplace_back(c);
}

void GameWorld::RemoveConstraint(Constraint* c, bool andDelete) {
	constraints.erase(std::remove(constraints.begin(), constraints.end(), c), constraints.end());
	if (andDelete) {
		delete c;
	}
}

void GameWorld::RemoveConstraint(std::vector<Constraint*>::const_iterator c, bool andDelete) {
	if (andDelete) {
		delete *c;
		constraints.erase(c);
	} else {
		constraints.erase(c);
	}
}

void GameWorld::GetConstraintIterators(
	std::vector<Constraint*>::const_iterator& first,
	std::vector<Constraint*>::const_iterator& last) const {
	first	= constraints.begin();
	last	= constraints.end();
}
