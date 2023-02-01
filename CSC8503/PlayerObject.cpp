#include "PlayerObject.h"

#include "AssetLibrary.h"
#include "Bullet.h"
#include "Bonus.h"
#include "Constraint.h"
#include "GameWorld.h"
#include "NPCObject.h"
#include "OrientationConstraint.h"
#include "PositionConstraint.h"
#include "PhysicsObject.h"
#include "Window.h"

#include "RenderObject.h"

#include <iostream>

using namespace NCL;
using namespace CSC8503;

PlayerObject::PlayerObject(GameWorld& gameWorld, int id, int& scoreCounter) : GameObject(gameWorld),
id(id), scoreCounter(scoreCounter),
behaviourRoot(std::string("Root-Player").append(std::to_string(id))),
groundAirSelector(std::string("GroundAir-Player").append(std::to_string(id))) {
	OnCollisionBeginCallback = [&](GameObject* other) {
		CollisionWith(other);
	};

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

	tounge = new GameObject(gameWorld, std::string("Tounge-Player").append(std::to_string(id)));
	tounge->GetTransform().SetScale(Vector3(0.1f, 1.0f, 0.1f));
	tounge->SetRenderObject(new RenderObject(&tounge->GetTransform(), AssetLibrary::GetMesh("cube"), nullptr, AssetLibrary::GetShader("basic")));
	tounge->GetRenderObject()->SetColour(Vector4(1, 0.8f, 0.8f, 1));
	tounge->SetActive(false);
	gameWorld.AddGameObject(tounge);

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
	transform.SetPosition(Vector3(transform.GetPosition().x, std::max(1.0f, transform.GetPosition().y), transform.GetPosition().z));

	lastGoosed += dt;

	groundTrigger->GetTransform().SetPosition(transform.GetPosition() + Vector3(0, -1, 0));
	behaviourRoot.Reset();
	while (behaviourRoot.Execute(dt) == Ongoing) {}
}

void PlayerObject::AddPoints(int points) {
	if (lastGoosed > gooseDelay) {
		lastGoosed = 0.0f;
		scoreCounter += points;
	}
}

void PlayerObject::CollisionWith(GameObject* other) {
	if (other->GetPhysicsObject() != nullptr && !collidedWith.contains(other->GetWorldID()) && !other->GetPhysicsObject()->IsStatic()) {
		collidedWith.insert(other->GetWorldID());
		if (dynamic_cast<NPCObject*>(other)) {
			scoreCounter += 500;
		} else {
			scoreCounter += 100;
		}
	}
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
	if (laserDelay < 0 && Window::GetKeyboard()->KeyDown(KeyboardKeys::R)) {
		laserDelay = laserFireRate;
		FireLasers();
	}

	if (grappledObject) {
		Vector3 toungePosition = transform.GetPosition() + transform.GetOrientation() * toungePos;
		Vector3 contactPosition = grappledObject->GetTransform().GetPosition() + toungeContactPoint;
		Vector3 delta = contactPosition - toungePosition;
		Vector3 deltaNorm = delta.Normalised();
		float distance = delta.Length();
		static const float as = std::sin(3.14159 * 0.25), ac = std::cos(3.14159 * 0.25);
		tounge->GetTransform().SetPosition(toungePosition + delta * 0.5f)
			                  .SetOrientation(Quaternion(Vector3::Cross(deltaNorm, Vector3(0, 1, 0)) * as, ac))
			                  .SetScale(Vector3(0.1f, distance, 0.1f));
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::E)) { // Release grappled object
			gameWorld.RemoveConstraint(grappleConstraint, true);
			grappledObject = nullptr;
			tounge->SetActive(false);
		}
	} else {
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::E)) { // Grapple a new object
			RayCollision collision;
			Ray r = Ray(transform.GetPosition() + toungePos, transform.GetOrientation() * Vector3(0, 0, -1), CollisionLayer::Player);
			if (gameWorld.Raycast(r, collision, true, this) && collision.rayDistance <= toungeMaxDistance) {
				grappledObject = (GameObject*)collision.node;
				toungeContactPoint = collision.collidedAt - grappledObject->GetTransform().GetPosition();
				grappleConstraint = new PositionConstraint(this, grappledObject, 0, toungeMaxDistance, Vector3(0), toungeContactPoint);
				gameWorld.AddConstraint(grappleConstraint);
				tounge->SetActive(true);

				CollisionWith(grappledObject);
			}
		}
	}
}

void PlayerObject::FireLasers() {
	Bullet* laserA = new Bullet(gameWorld, *(Bullet*)AssetLibrary::GetPrefab("bullet"));
	laserA->SetLifespan(laserLifespan);
	laserA->GetTransform().SetPosition(transform.GetOrientation() * eyePosL + transform.GetPosition());
	laserA->GetPhysicsObject()->AddForce(transform.GetOrientation() * laserForce);
	gameWorld.AddGameObject(laserA);
	laserA->OnCollisionBeginCallback = [&](GameObject* other) {
		CollisionWith(other);
	};

	Bullet* laserB = new Bullet(gameWorld, *(Bullet*)AssetLibrary::GetPrefab("bullet"));
	laserB->SetLifespan(laserLifespan);
	laserB->GetTransform().SetPosition(transform.GetOrientation() * eyePosR + transform.GetPosition());
	laserB->GetPhysicsObject()->AddForce(transform.GetOrientation() * laserForce);
	gameWorld.AddGameObject(laserB);
	laserB->OnCollisionBeginCallback = [&](GameObject* other) {
		CollisionWith(other);
	};
}
