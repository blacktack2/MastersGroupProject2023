#include "PlayerObject.h"

#include "Bullet.h"
#include "Constraint.h"
#include "GameWorld.h"
#include "OrientationConstraint.h"
#include "PositionConstraint.h"
#include "PhysicsObject.h"
#include "Window.h"

#include "RenderObject.h"

#include <iostream>

using namespace NCL;
using namespace CSC8503;

PlayerObject::PlayerObject(GameWorld& gameWorld, int id, Bullet& bulletPrefab) : GameObject(gameWorld),
id(id), bulletPrefab(bulletPrefab), behaviourRoot(std::string("Root-Player").append(std::to_string(id))), groundAirSelector(std::string("GroundAir-Player").append(std::to_string(id))) {
	groundTrigger = new GameObject(gameWorld, std::string("GroundTrigger-Player").append(std::to_string(id)));
	groundTrigger->GetTransform().SetScale(Vector3(0.1f));
	groundTrigger->SetBoundingVolume((CollisionVolume*)new AABBVolume(Vector3(0.1f), CollisionLayer::PlayerTrig));
	groundTrigger->SetPhysicsObject(new PhysicsObject(&groundTrigger->GetTransform(), groundTrigger->GetBoundingVolume(), true));
	groundTrigger->GetPhysicsObject()->SetInverseMass(0);
	groundTrigger->GetPhysicsObject()->SetElasticity(0);
	groundTrigger->GetPhysicsObject()->InitAxisAlignedInertia();
	groundTrigger->OnTriggerBeginCallback = [&](GameObject* other) { groundTriggerOverlaps++; };
	groundTrigger->OnTriggerEndCallback = [&](GameObject* other) { groundTriggerOverlaps--; };
	gameWorld.AddGameObject(groundTrigger);

	groundOrientationConstraint = new OrientationConstraint(this, Vector3(0, 1, 0));

	BehaviourAction* groundedActions = new BehaviourAction(std::string("Grounded-Player").append(std::to_string(id)),
		[&](float dt, BehaviourState state)->BehaviourState {
			switch (state) {
				case Initialise:
					if (groundTriggerOverlaps == 0) {
						return Failure;
					} else {
						if (!isGrounded) {
							gameWorld.AddConstraint(groundOrientationConstraint);
							isGrounded = true;
							std::cout << "Grounded\n";
						}
						HandleGroundInput(dt);
						return Success;
					}
				case Ongoing: case Success: case Failure: default:
					return state;
			}
		}
	);

	BehaviourAction* airborneActions = new BehaviourAction(std::string("Airborne-Player").append(std::to_string(id)),
		[&](float dt, BehaviourState state)->BehaviourState {
			switch (state) {
				case Initialise:
					if (groundTriggerOverlaps == 0) {
						if (isGrounded) {
							gameWorld.RemoveConstraint(groundOrientationConstraint);
							isGrounded = false;
							std::cout << "Airborn\n";
						}
						return Success;
					} else {
						return Failure;
					}
				case Ongoing: case Success: case Failure: default:
					return state;
			}
		}
	);

	BehaviourAction* goatActions = new BehaviourAction(std::string("GoatActions-Player").append(std::to_string(id)),
		[&](float dt, BehaviourState state)->BehaviourState {
			HandleGoatActions(dt);
			return Success;
		}
	);

	groundAirSelector.AddChild(groundedActions);
	groundAirSelector.AddChild(airborneActions);

	behaviourRoot.AddChild(&groundAirSelector);
	behaviourRoot.AddChild(goatActions);
}

PlayerObject::~PlayerObject() {

}

void PlayerObject::Update(float dt) {
	groundTrigger->GetTransform().SetPosition(transform.GetPosition() + Vector3(0, -1, 0));
	behaviourRoot.Reset();
	while (behaviourRoot.Execute(dt) == Ongoing) {}
}

void PlayerObject::HandleGroundInput(float dt) {
	const float moveForce = 40;
	const float rotateTorque = 4;
	const float jumpForce = 1000;
	const Vector3 jumpTorque = Vector3(1, 0, 1) * 500;
	float forwardThrust = 0;
	float upwardThrust = 0;
	Vector3 rotation = Vector3(0);
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A)) {
		rotation.y += rotateTorque;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D)) {
		rotation.y -= rotateTorque;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W)) {
		forwardThrust -= moveForce;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S)) {
		forwardThrust += moveForce;
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE)) {
		upwardThrust += jumpForce;
		rotation += Vector3((rand() * (1.0f / (float)RAND_MAX)), 0, (rand() * (1.0f / (float)RAND_MAX))) * jumpTorque;
	}
	physicsObject->AddForce(transform.GetOrientation() * Vector3(0, 0, forwardThrust) + Vector3(0, upwardThrust, 0));
	physicsObject->AddTorque(rotation);
}

void PlayerObject::HandleGoatActions(float dt) {
	laserDelay -= dt;
	if (laserDelay < 0 && Window::GetKeyboard()->KeyDown(KeyboardKeys::E)) {
		laserDelay = laserFireRate;
		FireLasers();
	}
}

void PlayerObject::FireLasers() {
	Bullet* laserA = new Bullet(bulletPrefab);
	laserA->SetLifespan(laserLifespan);
	laserA->GetTransform().SetPosition(transform.GetOrientation() * eyePosL + transform.GetPosition());
	laserA->GetPhysicsObject()->AddForce(transform.GetOrientation() * laserForce);
	gameWorld.AddGameObject(laserA);

	Bullet* laserB = new Bullet(bulletPrefab);
	laserB->SetLifespan(laserLifespan);
	laserB->GetTransform().SetPosition(transform.GetOrientation() * eyePosR + transform.GetPosition());
	laserB->GetPhysicsObject()->AddForce(transform.GetOrientation() * laserForce);
	gameWorld.AddGameObject(laserB);
}
