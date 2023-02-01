#include "PositionConstraint.h"
//#include "../../Common/Vector3.h"
#include "GameObject.h"
#include "PhysicsObject.h"
//#include "Debug.h"

using namespace NCL;
using namespace Maths;
using namespace CSC8503;

PositionConstraint::PositionConstraint(GameObject* a, GameObject* b, float d) {
	objectA  = a;
	objectB  = b;
	distance = d;

	currentAction = &PositionConstraint::UpdateDistanced;
}

PositionConstraint::PositionConstraint(GameObject* a, GameObject* b, Vector3 o) {
	objectA        = a;
	objectB        = b;
	positionOffset = o;

	currentAction = &PositionConstraint::UpdateOffset;
}

PositionConstraint::~PositionConstraint() {

}

//a simple constraint that stops objects from being more than <distance> away
//from each other...this would be all we need to simulate a rope, or a ragdoll
void PositionConstraint::UpdateConstraint(float dt)	{
	(*this.*currentAction)(dt);
}

void PositionConstraint::UpdateDistanced(float dt) {
	Vector3 relativePos = objectA->GetTransform().GetPosition() - objectB->GetTransform().GetPosition();
	float currentDistance = relativePos.Length();
	float offset = distance - currentDistance;

	if (std::abs(offset) > 0.0f) {
		Vector3 offsetDir = relativePos / currentDistance;

		PhysicsObject* physA = objectA->GetPhysicsObject();
		PhysicsObject* physB = objectB->GetPhysicsObject();

		float constraintMass = physA->GetInverseMass() + physB->GetInverseMass();

		if (constraintMass <= 0) {
			return;
		}
		Vector3 relativeVelocity = physA->GetLinearVelocity() - physB->GetLinearVelocity();

		float velocityDot = Vector3::Dot(relativeVelocity, offsetDir);
		float biasFactor = 0.01f;
		float bias = -(biasFactor / dt) * offset;

		float lagrangeMultiplier = -(velocityDot + bias) / constraintMass;

		Vector3 aImpulse = offsetDir * lagrangeMultiplier;
		Vector3 bImpulse = -offsetDir * lagrangeMultiplier;

		physA->ApplyLinearImpulse(aImpulse);
		physB->ApplyLinearImpulse(bImpulse);
	}
}

void PositionConstraint::UpdateOffset(float dt) {
	Vector3 actualPosition = objectA->GetTransform().GetPosition() - objectB->GetTransform().GetPosition();
	Vector3 expectedPosition = objectA->GetTransform().GetPosition() + positionOffset;

	Vector3 positionDelta = expectedPosition - actualPosition;

	if (positionDelta == Vector3(0)) {
		return;
	}

	float offset = positionDelta.Length();

	Vector3 offsetDir = actualPosition.Normalised();

	PhysicsObject* physA = objectA->GetPhysicsObject();
	PhysicsObject* physB = objectB->GetPhysicsObject();

	float constraintMass = physA->GetInverseMass() + physB->GetInverseMass();

	if (constraintMass <= 0) {
		return;
	}
	Vector3 relativeVelocity = physA->GetLinearVelocity() - physB->GetLinearVelocity();

	float velocityDot = Vector3::Dot(relativeVelocity, offsetDir);
	float biasFactor = 0.01f;
	float bias = -(biasFactor / dt) * offset;

	float lagrangeMultiplier = -(velocityDot + bias) / constraintMass;

	Vector3 aImpulse = offsetDir * lagrangeMultiplier;
	Vector3 bImpulse = -offsetDir * lagrangeMultiplier;

	physA->ApplyLinearImpulse(aImpulse);
	physB->ApplyLinearImpulse(bImpulse);
}
