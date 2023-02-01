#include "EnemyObject.h"

#include "Bullet.h"
#include "GameWorld.h"
#include "PhysicsObject.h"
#include "PlayerObject.h"

using namespace NCL;
using namespace CSC8503;

EnemyObject::EnemyObject(GameWorld& gameWorld, PlayerObject& player, NavigationMap& navMap) : GameObject(gameWorld),
player(player), navMap(navMap), stateMachine(new PatrolEnemyState(*this)) {
	OnCollisionBeginCallback = [&](GameObject* other) {
		if (dynamic_cast<Bullet*>(other)) {
			lastHitByBullet = 0;
		} else if (PlayerObject * player = dynamic_cast<PlayerObject*>(other)) {
			player->AddPoints(-500);
		}
	};
}

EnemyObject::~EnemyObject() {
}

void EnemyObject::Update(float dt) {
	transform.SetPosition(Vector3(transform.GetPosition().x, std::max(1.0f, transform.GetPosition().y), transform.GetPosition().z));

	lastHitByBullet += dt;
	stateMachine.Update(dt);
}

void EnemyObject::Daze() {
	player.AddPoints(50);
}

bool EnemyObject::CanSeePlayer() {
	RayCollision collision;
	Ray r = Ray(transform.GetPosition(), (player.GetTransform().GetPosition() - transform.GetPosition()).Normalised(), CollisionLayer::Enemy);
	return gameWorld.Raycast(r, collision, true, this) && collision.node == &player;
}

std::vector<Vector3> EnemyObject::GetRandomPatrolPoint() {
	NavigationPath outPath;
	int baseCounter = 20;
	while (!navMap.FindPath(transform.GetPosition(), Vector3(rand() % 200, 0, rand() % 200), outPath) && --baseCounter >= 0) {

	}
	Vector3 pos;
	std::vector<Vector3> waypoints;
	while (outPath.PopWaypoint(pos)) {
		waypoints.push_back(pos);
	}
	return waypoints;
}

void EnemyObject::FollowPlayer(float speed) {
	MoveTo(player.GetTransform().GetPosition(), speed);
}

void EnemyObject::MoveTo(const Vector3& position, float speed) {
	Vector3 delta = position - transform.GetPosition();
	delta.y = 0;
	delta.Normalise();
	physicsObject->AddForce(delta * speed);
}

float EnemyObject::SquareDistanceFrom(const Vector3& position) {
	return (Vector3(transform.GetPosition().x, 0, transform.GetPosition().z) - position).LengthSquared();
}
