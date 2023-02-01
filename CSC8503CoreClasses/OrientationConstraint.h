#pragma once
#include "Constraint.h"
#include "Transform.h"

#include <functional>

namespace NCL {
	namespace CSC8503 {
		class GameObject;

		class OrientationConstraint : public Constraint {
		public:
			OrientationConstraint(GameObject* primary, GameObject* hinge, Vector3 direction = Vector3(0, 0, 1));
			OrientationConstraint(GameObject* primary, Vector3 dir);
			~OrientationConstraint();

			void UpdateConstraint(float dt) override;
		protected:
			typedef void (OrientationConstraint::*constraint_action)(float);

			void UpdateLookat(float dt);
			void UpdateFixed(float dt);

			constraint_action currentAction;

			GameObject* primaryObject;
			GameObject* hingeObject;

			Vector3 direction;
		};
	}
}

