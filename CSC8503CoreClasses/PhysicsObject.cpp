#include "PhysicsObject.h"
#include "PhysicsSystem.h"
#include "Transform.h"
using namespace NCL;
using namespace CSC8503;

PhysicsObject::PhysicsObject(Transform* parentTransform, const CollisionVolume* parentVolume, bool trigger)	{
	transform	= parentTransform;
	volume		= parentVolume;

	inverseMass = 1.0f;
	elasticity	= 0.8f;
	friction	= 0.8f;
	gravWeight  = 1.0f;

	isTrigger = trigger;
	isStatic = false;
}

PhysicsObject::PhysicsObject(PhysicsObject& other, Transform* parentTransform) {
	transform = parentTransform;
	volume = other.volume;

	isTrigger = other.isTrigger;
	isStatic  = other.isStatic;

	inverseMass = other.inverseMass;
	elasticity = other.elasticity;
	friction = other.friction;
	gravWeight = other.gravWeight;

	linearVelocity = other.linearVelocity;
	force = other.force;

	angularVelocity = other.angularVelocity;
	torque = other.torque;
	inverseInertia = other.inverseInertia;
	inertiaTensor = other.inertiaTensor;
	inverseInteriaTensor = other.inverseInteriaTensor;
}

PhysicsObject::~PhysicsObject()	{

}

void PhysicsObject::ApplyAngularImpulse(const Vector3& force) {
	angularVelocity += inverseInteriaTensor * force;
}

void PhysicsObject::ApplyLinearImpulse(const Vector3& force) {
	linearVelocity += force * inverseMass;
}

void PhysicsObject::AddForce(const Vector3& addedForce) {
	force += addedForce;
}

void PhysicsObject::AddForceAtPosition(const Vector3& addedForce, const Vector3& position) {
	Vector3 localPos = position - transform->GetPosition();

	force  += addedForce;
	torque += Vector3::Cross(localPos, addedForce);
}

void PhysicsObject::AddTorque(const Vector3& addedTorque) {
	torque += addedTorque;
}

void PhysicsObject::ClearForces() {
	force				= Vector3();
	torque				= Vector3();
}

void NCL::CSC8503::PhysicsObject::InitAxisAlignedInertia() {
	inverseInertia = Vector3(0.0f);
}

void PhysicsObject::InitCubeInertia() {
	Vector3 dimensions	= transform->GetScale();

	Vector3 fullWidth = dimensions * 2;

	Vector3 dimsSqr		= fullWidth * fullWidth;

	inverseInertia.x = (12.0f * inverseMass) / (dimsSqr.y + dimsSqr.z);
	inverseInertia.y = (12.0f * inverseMass) / (dimsSqr.x + dimsSqr.z);
	inverseInertia.z = (12.0f * inverseMass) / (dimsSqr.x + dimsSqr.y);
}

void PhysicsObject::InitSphereInertia() {
	float radius	= transform->GetScale().GetMaxElement();
	float i			= 2.5f * inverseMass / (radius*radius);

	inverseInertia	= Vector3(i, i, i);
}

void PhysicsObject::InitSphereInertiaHollow() {
	float radius	= transform->GetScale().GetMaxElement();
	float i			= 1.5f * inverseMass / (radius*radius);

	inverseInertia	= Vector3(i, i, i);
}

void PhysicsObject::InitCapsuleInertia() {
	float height = transform->GetScale().y;
	float radius = std::max(transform->GetScale().x, transform->GetScale().z) / 2;
	float xz = 12.0f * inverseMass / (3.0f * 3.14159f * height * radius * radius * ((height * height * (1.0f / 3.0f)) + (radius * radius)) + 16.0f * 3.14159f * radius * radius * radius * ((0.4f * radius * radius) + (0.5f * height * height) + (0.375 * height * radius)));
	float y = 30.0f * inverseMass / (radius * radius * radius * radius * 3.14159f * (15.0f * height + 8.0f * radius));

	inverseInertia = Vector3(xz, y, xz);
}

void PhysicsObject::UpdateInertiaTensor() {
	Quaternion q = transform->GetOrientation();
	
	Matrix3 invOrientation	= Matrix3(q.Conjugate());
	Matrix3 orientation		= Matrix3(q);

	inverseInteriaTensor = orientation * Matrix3::Scale(inverseInertia) * invOrientation;
	Vector3 inertia = Vector3(1.0f / inverseInertia.x, 1.0f / inverseInertia.y, 1.0f / inverseInertia.z);
	inertiaTensor = invOrientation * Matrix3::Scale(inertia) * orientation;
}
