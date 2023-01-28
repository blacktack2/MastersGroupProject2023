#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <glm/ext/quaternion_float.hpp>

using namespace glm;
namespace paintHell::core {
	class Transform {
	public:
		Transform();
		~Transform();

		const mat4 GetLocalMatrix() { return localMatrix; }
		const mat4 GetWorldMatrix() { return worldMatrix; }
		Transform* GetParent() { return parent; }

		const vec4 GetLocalPosition() { return position; }
		const vec3 GetScale() { return scale; }
		const quat GetRotation() { return rotation; }

		const vec4 GetWorldPosition();

		Transform& SetLocalPosition(const vec4& newPos);
		Transform& SetLocalPosition(const vec3& newPos) { return SetLocalPosition(vec4(newPos, 1.0)); }
		Transform& SetScale(const vec3& newScale);
		Transform& SetRotation(const quat& newRot);
		Transform& SetParent(Transform* newParent) { parent = newParent; UpdateWorldMatrix(); return *this; }




		void UpdateWorldMatrix();

		void DebugLog();
		
	private:
		void UpdateLocalMatrix();
		void LogMatrix(mat4 matrix);

	private:
		vec4 position;
		vec3 scale;

		quat rotation;
	
		mat4 localMatrix;
		mat4 worldMatrix;
		Transform* parent;
		std::vector<Transform*> children;
	};
}