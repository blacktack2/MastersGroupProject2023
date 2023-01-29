#include "Transform.h"
#include "GameObject.h"
#include <glm/gtx/string_cast.hpp>
#include <iostream>

using namespace paintHell::core;


/*Transform::Transform() {
	scale = glm::vec3(1, 1, 1);
	position = glm::vec4(0, 0, 0, 1);
	rotation = glm::quat(0, 0, 0, 0);
	parent = nullptr;
	UpdateLocalMatrix();
}*/

Transform::Transform(GameObject* gameObject) {
	scale = glm::vec3(1, 1, 1);
	position = glm::vec4(0, 0, 0, 1);
	rotation = glm::quat(0, 0, 0, 0);
	parent = nullptr;
	this->gameObject = gameObject;
	UpdateLocalMatrix();
}

Transform::~Transform() {

}

Transform& Transform::SetLocalPosition(const glm::vec4& newPos) {
	position = newPos;
	UpdateLocalMatrix();
	return *this;
}

Transform& Transform::SetScale(const glm::vec3& newScale) {
	scale = newScale;
	UpdateLocalMatrix();
	return *this;
}
Transform& Transform::SetRotation(const glm::quat& newRot) {
	rotation = newRot;
	UpdateLocalMatrix();
	return *this;
}

const glm::vec4 Transform::GetWorldPosition() {
	return parent ? parent->GetWorldMatrix() * position : position;
}





void Transform::UpdateLocalMatrix() {
	glm::mat4 rotationMatrix = glm::mat4(rotation); //likely can be better optimised for memory
	localMatrix = glm::translate(glm::identity<glm::mat4>(), glm::vec3(position)) * rotationMatrix;
	localMatrix = glm::scale(localMatrix, scale);

	UpdateWorldMatrix();
}

void Transform::UpdateWorldMatrix() {
	worldMatrix = parent ? parent->GetWorldMatrix() * localMatrix : localMatrix;

	for (std::vector<Transform*>::iterator child = children.begin(); child != children.end(); child++)
	{
		(*child)->UpdateWorldMatrix();
	}
}

void Transform::DebugLog() {
	std::cout << "Position: " << glm::to_string(position) << "\n";
	std::cout << "Scale: " << glm::to_string(scale) << "\n";

	std::cout << "localMatrix:\n";
	LogMatrix(localMatrix);
	std::cout << "worldMatrix:\n";
	LogMatrix(worldMatrix);
}

void Transform::LogMatrix(glm::mat4 matrix) {
	for (int col = 0; col < 4; ++col) {
		std::cout << "| ";
		for (int row = 0; row < 4; ++row) {
			std::cout << matrix[row][col] << '\t';
		}
		std::cout << '\n';
	}
	std::cout << '\n';
}