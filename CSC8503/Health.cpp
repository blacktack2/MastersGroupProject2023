#include "Health.h"


using namespace NCL;
using namespace CSC8503;

Health::Health() {
}
Health::Health(float health) {
	this->health = health;
	this->maxHealth = health;
}

Health::~Health() {
}

void Health::Update(float dt) {
	overTimeTimer -= dt;
}

