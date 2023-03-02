#include "Health.h"


using namespace NCL;
using namespace CSC8503;

Health::Health() : health(100) {
}
Health::Health(float health) {
	this->health = health;
}

Health::~Health() {
}

void Health::Update(float dt) {
	overTimeTimer -= dt;
}

