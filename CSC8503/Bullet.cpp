#include "Bullet.h"

#include "CapsuleVolume.h"

#include "Debug.h"

using namespace NCL;
using namespace CSC8503;

Bullet::Bullet(GameWorld& gameWorld) : GameObject(gameWorld) {
}

Bullet::Bullet(GameWorld& gameWorld, Bullet& other) : GameObject(gameWorld, other) {
	lifespan = other.lifespan;
}

Bullet::~Bullet() {
}

void Bullet::Update(float dt) {
	lifespan -= dt;
	if (lifespan <= 0) {
		Delete();
		return;
	}

	Debug::DrawLine(transform.GetPosition(), transform.GetPosition() + Vector3(0, 1, 0), Vector4(0, 1, 1, 1));
}
