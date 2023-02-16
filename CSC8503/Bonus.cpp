#include "Bonus.h"

#include "GameWorld.h"
#include "PhysicsObject.h"
#include "PlayerObject.h"

using namespace NCL;
using namespace CSC8503;

static int id = 0;

BonusObject::BonusObject() : GameObject(std::string("Bonus").append(std::to_string(id++))) {
	OnTriggerBeginCallback = [&](GameObject* other) {
		if (PlayerObject* player = dynamic_cast<PlayerObject*>(other)) {
			Delete();
		}
	};
}

BonusObject::~BonusObject() {
}

void BonusObject::Update(float dt) {
	physicsObject->ApplyAngularImpulse(Vector3(dt, 1, 0));
}
