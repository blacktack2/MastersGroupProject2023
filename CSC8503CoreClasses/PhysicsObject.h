#pragma once
using namespace NCL::Maths;

namespace NCL {
	class CollisionVolume;
	
	namespace CSC8503 {
		class Transform;

		class PhysicsObject	{
		public:
			PhysicsObject(Transform* parentTransform, const CollisionVolume* parentVolume, bool isTrigger = false);
			PhysicsObject(PhysicsObject& other, Transform* parentTransform);
			~PhysicsObject();

			Vector3 GetLinearVelocity() const {
				return linearVelocity;
			}

			Vector3 GetAngularVelocity() const {
				return angularVelocity;
			}

			Vector3 GetTorque() const {
				return torque;
			}

			Vector3 GetForce() const {
				return force;
			}

			void SetInverseMass(float invMass) {
				inverseMass = invMass;
			}

			float GetInverseMass() const {
				return inverseMass;
			}

			void SetElasticity(float elast) {
				elasticity = elast;
			}

			float GetElasticity() const {
				return elasticity;
			}

			void SetFriction(float frict) {
				friction = frict;
			}

			float GetFriction() const {
				return friction;
			}

			void SetGravWeight(float weight) {
				gravWeight = weight;
			}

			float GetGravWeight() const {
				return gravWeight;
			}

			void ApplyAngularImpulse(const Vector3& force);
			void ApplyLinearImpulse(const Vector3& force);
			
			void AddForce(const Vector3& force);

			void AddForceAtPosition(const Vector3& force, const Vector3& position);

			void AddTorque(const Vector3& torque);


			void ClearForces();

			void SetLinearVelocity(const Vector3& v) {
				linearVelocity = v;
			}

			void SetAngularVelocity(const Vector3& v) {
				angularVelocity = v;
			}

			void InitAxisAlignedInertia();
			void InitCubeInertia();
			void InitSphereInertia();
			void InitSphereInertiaHollow();
			void InitCapsuleInertia();

			void UpdateInertiaTensor();

			Matrix3 GetInertiaTensor() const {
				return inertiaTensor;
			}

			Matrix3 GetInverseInertiaTensor() const {
				return inverseInteriaTensor;
			}

			void SetStatic(bool s = true) {
				isStatic = s;
			}

			bool IsTrigger() const {
				return isTrigger;
			}
			bool IsStatic() const {
				return isStatic;
			}
		protected:
			const CollisionVolume* volume;
			Transform* transform;

			bool isTrigger;
			bool isStatic;

			float inverseMass;
			float elasticity;
			float friction;
			float gravWeight;

			//linear stuff
			Vector3 linearVelocity;
			Vector3 force;
			
			//angular stuff
			Vector3 angularVelocity;
			Vector3 torque;
			Vector3 inverseInertia;
			Matrix3 inertiaTensor;
			Matrix3 inverseInteriaTensor;
		};
	}
}
