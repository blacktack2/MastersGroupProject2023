#include "Transform.h"

using namespace core;
//using namespace glm;

Transform::Transform() {
	scale = glm::vec3(1, 1, 1);
}

Transform::~Transform() {

}

Transform& Transform::SetLocalPosition(const glm::vec3& newPos) {
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


void Transform::UpdateLocalMatrix() {
	mat4 rotationMatrix = glm::mat4(rotation); //likely can be better optimised for memory
	localMatrix = glm::translate(glm::identity<mat4>(), position) * rotationMatrix;
	localMatrix = glm::scale(localMatrix, scale);
}

void Transform::UpdateWorldMatrix() {
	worldMatrix = parent ? localMatrix * parent->GetWorldMatrix() : localMatrix;

	for (std::vector<Transform*>::iterator child = children.begin(); child != children.end(); child++)
	{
		(*child)->UpdateWorldMatrix();
	}
}