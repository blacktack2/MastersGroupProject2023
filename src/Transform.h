#pragma once
#include <glm/glm.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <vector>

using namespace glm;
namespace core {
	class Transform {
	public:
		Transform();
		~Transform();

		mat4 GetLocalMatrix() { return localMatrix; }
		mat4 GetWorldMatrix() { return worldMatrix; }
		Transform* GetParent() { return parent; }

		const vec3 GetPosition() { return position; }
		const vec3 GetScale() { return scale; }
		const quat GetRotation() { return rotation; }

		Transform& SetLocalPosition(const vec3& newPos);
		Transform& SetScale(const vec3& newScale);
		Transform& SetRotation(const quat& newRot);



		void UpdateWorldMatrix();

	private:
		void UpdateLocalMatrix();

	private:
		vec3 position;
		vec3 scale;

		quat rotation;
	
		mat4 localMatrix;
		mat4 worldMatrix;
		Transform* parent;
		std::vector<Transform*> children;
	};
}