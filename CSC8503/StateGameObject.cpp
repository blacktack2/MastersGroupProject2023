#include "StateGameObject.h"
#include "StateTransition.h"
#include "StateMachine.h"
#include "State.h"
#include "PhysicsObject.h"

using namespace NCL;
using namespace CSC8503;

StateGameObject::StateGameObject(GameWorld& gameWorld, std::string name) : GameObject(gameWorld, name) {
	counter = 0.0f;
	stateMachine = new StateMachine();

	State* stateA = new State([&](float dt)->void{
		MoveLeft(dt);
	});
	State* stateB = new State([&](float dt)->void{
		MoveRight(dt);
	});

	stateMachine->AddState(stateA);
	stateMachine->AddState(stateB);
	
	stateMachine->AddTransition(new StateTransition(stateA, stateB, [&]()->bool{
		return counter > 3.0f;
	}));
	stateMachine->AddTransition(new StateTransition(stateB, stateA, [&]()->bool{
		return counter < 0.0f;
	}));
}

StateGameObject::~StateGameObject() {
	delete stateMachine;
}

void StateGameObject::Update(float dt) {
	stateMachine->Update(dt);
}

void StateGameObject::MoveLeft(float dt) {
	GetPhysicsObject()->AddForce(Vector3(-50, 0, 0));
	counter += dt;
}

void StateGameObject::MoveRight(float dt) {
	GetPhysicsObject()->AddForce(Vector3(50, 0, 0));
	counter -= dt;
}