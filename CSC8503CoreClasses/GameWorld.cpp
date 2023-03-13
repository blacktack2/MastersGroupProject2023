#include "Camera.h"
#include "CollisionDetection.h"
#include "Constraint.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "PhysicsObject.h"

using namespace NCL;
using namespace NCL::CSC8503;

GameWorld::GameWorld() : staticQuadTree(Vector2(1024, 1024), 7, 6), dynamicQuadTree(Vector2(1024, 1024), 7, 6) {
	
	playerCamera1 = std::make_unique<Camera>();
	playerCamera2 = std::make_unique<Camera>();
	playerCamera3 = std::make_unique<Camera>();
	playerCamera4 = std::make_unique<Camera>();
	
	SetMainCamera(1);

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
	networkObjects.clear();
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
	if (o->GetNetworkObject() != nullptr) {
		AddNetworkObject(o->GetNetworkObject());
	}
}

void GameWorld::RemoveGameObject(GameObject* o, bool andDelete) {
	gameObjects.erase(std::remove(gameObjects.begin(), gameObjects.end(), o), gameObjects.end());
	if (andDelete) {
		delete o;
	}
	worldStateCounter++;
}

void GameWorld::AddNetworkObject(NetworkObject* o) {
	networkObjects.emplace_back(o);
}

void GameWorld::RemoveNetworkObject(NetworkObject* o) {
	networkObjects.erase(std::remove(networkObjects.begin(), networkObjects.end(), o), networkObjects.end());
}


void GameWorld::GetObjectIterators(
	GameObjectIterator& first,
	GameObjectIterator& last) const {

	first	= gameObjects.begin();
	last	= gameObjects.end();
}

void GameWorld::GetConstraintIterators(ConstraintIterator& first, ConstraintIterator& last) const {
	first = constraints.begin();
	last = constraints.end();
}

void GameWorld::GetLightIterators(LightIterator& first, LightIterator& last) const {
	first = lights.begin();
	last = lights.begin();
}

void GameWorld::OperateOnContents(GameObjectFunc f) {
	for (GameObject* g : gameObjects) {
		if (g->IsActive())
		{
			f(g);
		}
	}
}

void GameWorld::OperateOnConstraints(ConstraintFunc f) {
	for (Constraint* c : constraints) {
		f(c);
	}
}

void GameWorld::OperateOnLights(LightFunc f) {
	for (const Light* l : lights) {
		f(*l);
	}
}

void GameWorld::PreUpdateWorld() {
	std::vector<GameObject*> delObjs;

	gameObjects.erase(std::remove_if(gameObjects.begin(), gameObjects.end(), [](GameObject* g) {
		if (g->IsMarkedDelete()) {
			delete g;
			return true;
		}
		return false;
	}), gameObjects.end());
	UpdateDynamicTree();
}

void GameWorld::UpdateWorld(float dt) {
	deltaTime = dt;
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
		if (gameObjects[i]->IsActive())
		{
			gameObjects[i]->Update(dt);
		}
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
		if ((*i)->GetPhysicsObject() && !(*i)->GetPhysicsObject()->IsStatic() && (*i)->IsActive()) {
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

Camera* GameWorld::GetCamera(int n) const {
	switch (n) {
	case 0:
		return playerCamera4.get(); // TODO this is HARDCODED! fix this later!
	case 1:
		return playerCamera1.get();
	case 2:
		return playerCamera2.get();
	case 3:
		return playerCamera3.get();
	case 4:
		return playerCamera4.get();
	default:
		return nullptr;
	}
}

Camera* GameWorld::SetMainCamera(int n) {
	mainCameraIndex = n;
	switch (n) {
	case 1:
		mainCamera = playerCamera1.get();
		return mainCamera;
	case 2:
		mainCamera = playerCamera2.get();
		return mainCamera;
	case 3:
		mainCamera = playerCamera3.get();
		return mainCamera;
	case 4:
		mainCamera = playerCamera4.get();
		return mainCamera;
	default:
		return nullptr;
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

Light* GameWorld::AddLight(Light* l) {
	lights.push_back(l);
	return l;
}

void GameWorld::RemoveLight(LightIterator l) {
	lights.erase(l);
}

void GameWorld::ClearLight() {
	lights.clear();
}

void GameWorld::InitCameras() const {
	InitCamera(*playerCamera1);
	InitCamera(*playerCamera2);
	InitCamera(*playerCamera3);
	InitCamera(*playerCamera4);
}

void GameWorld::InitCamera(Camera& cam) const {
	cam.SetNearPlane(0.1f);
	cam.SetFarPlane(1000.0f);
	cam.SetPitch(-15.0f);
	cam.SetYaw(315.0f);
	cam.SetPosition(Vector3(-60, 40, 60));
	cam.SetFollow(nullptr);
}
