#pragma once
#include "Constraint.h"
#include "Vector3.h"

namespace NCL {
	using namespace Maths;
	namespace CSC8503 {
		class GameObject;

		class PositionConstraint : public Constraint	{
		public:
			PositionConstraint(GameObject* a, GameObject* b, float d);
			PositionConstraint(GameObject* a, GameObject* b, Vector3 offsetB);
			~PositionConstraint();

			void UpdateConstraint(float dt) override;
		protected:
			typedef void (PositionConstraint::*constraint_action)(float);

			void UpdateDistanced(float dt);
			void UpdateOffset(float dt);

			constraint_action currentAction;

			GameObject* objectA;
			GameObject* objectB;

			float distance;
			Vector3 positionOffset;
		};
	}
}