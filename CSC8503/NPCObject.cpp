#include "NPCObject.h"

#include "EnemyObject.h"
#include "GameWorld.h"
#include "OrientationConstraint.h"
#include "PhysicsObject.h"
#include "PlayerObject.h"
#include "StateTransition.h"
#include "State.h"

using namespace NCL;
using namespace CSC8503;

static int id = 0;

NPCObject::NPCObject(GameWorld& gameWorld) : GameObject(gameWorld, std::string("NPC").append(std::to_string(id++))), stateMachine() {
	perceptionTrigger = new GameObject(gameWorld, std::string("PerceptionTrigger-").append(name));
	perceptionTrigger->GetTransform().SetScale(Vector3(10.0f));
	perceptionTrigger->SetBoundingVolume((CollisionVolume*)new SphereVolume(10.0f));
	perceptionTrigger->SetPhysicsObject(new PhysicsObject(&perceptionTrigger->GetTransform(), perceptionTrigger->GetBoundingVolume(), true));
	perceptionTrigger->GetPhysicsObject()->SetInverseMass(0);
	perceptionTrigger->GetPhysicsObject()->InitAxisAlignedInertia();
	perceptionTrigger->OnTriggerBeginCallback = [&](GameObject* other) {
		if (objectOfFear == nullptr && (dynamic_cast<PlayerObject*>(other) || dynamic_cast<EnemyObject*>(other))) {
			objectOfFear = other;
		}
	};
	perceptionTrigger->OnTriggerEndCallback = [&](GameObject* other) {
		if (other == objectOfFear) {
			objectOfFear = nullptr;
		}
	};
	gameWorld.AddGameObject(perceptionTrigger);

	orientationConstraint = new OrientationConstraint(this, Vector3(0, 1, 0));
	gameWorld.AddConstraint(orientationConstraint);

	State* wanderState = new State(
		[&](float dt)->void {
			Wander(dt);
		}
	);

	State* fleeState = new State(
		[&](float dt)->void {
			Flee(dt);
		}
	);

	StateTransition* tranWanderToFlee = new StateTransition(wanderState, fleeState,
		[&](void)->bool {
			return objectOfFear != nullptr;
		}
	);

	StateTransition* tranFleeToWander = new StateTransition(fleeState, wanderState,
		[&](void)->bool {
			return objectOfFear == nullptr;
		}
	);

	stateMachine.AddState(wanderState);
	stateMachine.AddState(fleeState);
	stateMachine.AddTransition(tranWanderToFlee);
	stateMachine.AddTransition(tranFleeToWander);
}

NPCObject::~NPCObject() {
}

void NPCObject::Update(float dt) {
	transform.SetPosition(Vector3(transform.GetPosition().x, std::max(1.0f, transform.GetPosition().y), transform.GetPosition().z));

	perceptionTrigger->GetTransform().SetPosition(transform.GetPosition());

	stateMachine.Update(dt);
}

void NPCObject::Wander(float dt) {
	wanderDir += Vector3(((float)rand() / (float)RAND_MAX) * 0.2f - 0.1f, 0, ((float)rand() / (float)RAND_MAX) * 0.2f - 0.1f);
	wanderDir.Normalise();
	physicsObject->AddForce(wanderDir * wanderForce * dt);
}

void NPCObject::Flee(float dt) {
	if (objectOfFear == nullptr) {
		return;
	}
	Vector3 delta = transform.GetPosition() - objectOfFear->GetTransform().GetPosition();
	delta.y = 0;
	delta.Normalise();
	physicsObject->AddForce(delta * fleeForce);
}
