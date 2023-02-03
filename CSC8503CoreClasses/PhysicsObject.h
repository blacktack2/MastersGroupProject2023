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

			Vector3 GetTorque() const {
				return torque;
			}

			Vector3 GetForce() const {
				return force;
			}

			void ApplyAngularImpulse(const Vector3& force);
			void ApplyLinearImpulse(const Vector3& force);
			
			void AddForce(const Vector3& force);

			void AddForceAtPosition(const Vector3& force, const Vector3& position);

			void AddTorque(const Vector3& torque);


			void ClearForces();

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

			bool IsTrigger() const {
				return isTrigger;
			}

		public:

			bool isStatic;

			float inverseMass;
			float friction; //not used
			float elasticity; //not used
			float gravScale;

			float linearDamping;
			Vector3 linearVelocity;

			Vector3 angularVelocity;
		protected:
			const CollisionVolume* volume;
			Transform* transform;

			bool isTrigger;
			

			//linear stuff
			Vector3 force;
			
			//angular stuff
			Vector3 torque;
			Vector3 inverseInertia;
			Matrix3 inertiaTensor;
			Matrix3 inverseInteriaTensor;
		};
	}
}
