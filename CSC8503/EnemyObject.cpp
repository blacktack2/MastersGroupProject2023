#include "EnemyObject.h"

#include "Bullet.h"
#include "GameWorld.h"
#include "PhysicsObject.h"
#include "PlayerObject.h"

using namespace NCL;
using namespace CSC8503;

EnemyObject::EnemyObject(PlayerObject& player, NavigationMap& navMap) : GameObject(),
player(player), navMap(navMap), stateMachine(new PatrolEnemyState(*this)) {
	OnCollisionBeginCallback = [&](GameObject* other) {
		if (dynamic_cast<Bullet*>(other)) {
			lastHitByBullet = 0;
		} else if (PlayerObject * player = dynamic_cast<PlayerObject*>(other)) {
			
		}
	};
}

EnemyObject::~EnemyObject() {
}

void EnemyObject::Update(float dt) {
	transform.SetPosition(Vector3(transform.GetGlobalPosition().x, std::max(1.0f, transform.GetGlobalPosition().y), transform.GetGlobalPosition().z));

	lastHitByBullet += dt;
	stateMachine.Update(dt);
}

void EnemyObject::Daze() {
	
}

bool EnemyObject::CanSeePlayer() {
	RayCollision collision;
	Ray r = Ray(transform.GetGlobalPosition(), (player.GetTransform().GetGlobalPosition() - transform.GetGlobalPosition()).Normalised(), CollisionLayer::Enemy);
	return gameWorld.Raycast(r, collision, true, this) && collision.node == &player;
}

std::vector<Vector3> EnemyObject::GetRandomPatrolPoint() {
	NavigationPath outPath;
	int baseCounter = 20;
	while (!navMap.FindPath(transform.GetGlobalPosition(), Vector3((float)(rand() % 200), 0, (float)(rand() % 200)), outPath) && --baseCounter >= 0) {

	}
	Vector3 pos;
	std::vector<Vector3> waypoints;
	while (outPath.PopWaypoint(pos)) {
		waypoints.push_back(pos);
	}
	return waypoints;
}

void EnemyObject::FollowPlayer(float speed) {
	MoveTo(player.GetTransform().GetGlobalPosition(), speed);
}

void EnemyObject::MoveTo(const Vector3& position, float speed) {
	Vector3 delta = position - transform.GetGlobalPosition();
	delta.y = 0;
	delta.Normalise();
	physicsObject->AddForce(delta * speed);
}

float EnemyObject::SquareDistanceFrom(const Vector3& position) {
	return (Vector3(transform.GetGlobalPosition().x, 0, transform.GetGlobalPosition().z) - position).LengthSquared();
}
