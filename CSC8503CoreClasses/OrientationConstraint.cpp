#include "OrientationConstraint.h"

#include "GameObject.h"
#include "PhysicsObject.h"

using namespace NCL;
using namespace Maths;
using namespace CSC8503;

OrientationConstraint::OrientationConstraint(GameObject* primary, GameObject* hinge, Vector3 forward) {
	hingeObject = hinge;
	primaryObject = primary;
	direction = forward;

	currentAction = &OrientationConstraint::UpdateLookat;
}

OrientationConstraint::OrientationConstraint(GameObject* primary, Vector3 dir) {
	primaryObject = primary;
	direction = dir;

	currentAction = &OrientationConstraint::UpdateFixed;
}

OrientationConstraint::~OrientationConstraint() {

}

void OrientationConstraint::UpdateConstraint(float dt) {
	(*this.*currentAction)(dt);
}

void OrientationConstraint::UpdateLookat(float dt) {
	Vector3 relativePos = hingeObject->GetTransform().GetPosition() - primaryObject->GetTransform().GetPosition();

	Vector3 expectedDirection = relativePos.Normalised();
	Vector3 actualDirection = hingeObject->GetTransform().GetOrientation() * direction;

	Vector3 directionDelta = expectedDirection - actualDirection;

	if (directionDelta == Vector3(0)) {
		return;
	}

	float offset = directionDelta.Length();

	PhysicsObject* physB = hingeObject->GetPhysicsObject();

	Matrix3 inertia = physB->GetInertiaTensor();

	float biasFactor = 0.0001f;
	float bias = -(biasFactor / dt) * offset;

	Vector3 angularImpulse = (inertia * Vector3::Cross(directionDelta, expectedDirection)) * bias;

	physB->ApplyAngularImpulse(angularImpulse);
}

void OrientationConstraint::UpdateFixed(float dt) {
	Vector3 actualDirection = primaryObject->GetTransform().GetOrientation() * direction;

	Vector3 directionDelta = direction - actualDirection;

	if (directionDelta == Vector3(0)) {
		return;
	}

	float offset = directionDelta.Length();

	PhysicsObject* phys = primaryObject->GetPhysicsObject();

	Matrix3 inertia = phys->GetInertiaTensor();

	float biasFactor = 0.0001f;
	float bias = -(biasFactor / dt) * offset;

	Vector3 angularImpulse = (inertia * Vector3::Cross(directionDelta, direction)) * bias;

	phys->ApplyAngularImpulse(angularImpulse);
}
