#pragma once
#include "Matrix4.h"
#include "Quaternion.h"
#include "Vector3.h"
#include <vector>

namespace NCL {
	using namespace Maths;

	namespace CSC8503 {
		class GameObject;

		class Transform
		{
		public:
			Transform(GameObject* gameoOject);
			~Transform();

			Transform& SetPosition(const Vector3& worldPos);
			Transform& SetScale(const Vector3& worldScale);
			Transform& SetOrientation(const Quaternion& newOr);

			Vector3 GetGlobalPosition() const {
				return parent ? parent->GetGlobalMatrix() * position : position;
			}

			Vector3 GetLocalPosition() const {
				return position;
			}

			Vector3 GetScale() const {
				return scale;
			}

			Quaternion GetGlobalOrientation() const {
				return globalOrientation;
			}

			Matrix4 GetGlobalMatrix() const {
				return globalMatrix;
			}

			

			void UpdateGlobalOrientation();
			void UpdateGlobalMatrix();
			void UpdateLocalMatrix();

			void RemoveChild(Transform* child);
			void AddChild(Transform* child);

		protected:
			void SetParent(Transform* transform) { parent = transform; UpdateGlobalMatrix(); UpdateGlobalOrientation(); }

			Matrix4		globalMatrix;
			Matrix4		localMatrix;
			Quaternion	orientation;
			Quaternion globalOrientation;
			Vector3		position;

			Vector3		scale;

			Transform* parent;
			std::vector<Transform*> children;
			GameObject* gameObject;
		};
	}
}

