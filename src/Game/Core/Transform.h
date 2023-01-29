#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <glm/ext/quaternion_float.hpp>

namespace paintHell::core {
	class GameObject;

	class Transform {
	public:
		//Transform();
		Transform(GameObject* gameObject);
		~Transform();

		const glm::mat4 GetLocalMatrix() { return localMatrix; }
		const glm::mat4 GetWorldMatrix() { return worldMatrix; }
		Transform* GetParent() { return parent; }

		const glm::vec4 GetLocalPosition() { return position; }
		const glm::vec3 GetScale() { return scale; }
		const glm::quat GetRotation() { return rotation; }

		const glm::vec4 GetWorldPosition();

		Transform& SetLocalPosition(const glm::vec4& newPos);
		Transform& SetLocalPosition(const glm::vec3& newPos) { return SetLocalPosition(glm::vec4(newPos, 1.0)); }
		Transform& SetScale(const glm::vec3& newScale);
		Transform& SetRotation(const glm::quat& newRot);
		Transform& SetParent(Transform* newParent) { parent = newParent; UpdateWorldMatrix(); return *this; }




		void UpdateWorldMatrix();

		void DebugLog();
		
	private:
		void UpdateLocalMatrix();
		void LogMatrix(glm::mat4 matrix);

	private:
		glm::vec4 position;
		glm::vec3 scale;

		glm::quat rotation;
	
		glm::mat4 localMatrix;
		glm::mat4 worldMatrix;
		Transform* parent;
		std::vector<Transform*> children;
		GameObject* gameObject;
	};
}