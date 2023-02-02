#include "Transform.h"

using namespace NCL;
using namespace CSC8503;

Transform::Transform(GameObject* gameObject){
	this->gameObject = gameObject;
	this->parent = NULL;
	scale = Vector3(1, 1, 1);
}

Transform::~Transform()	{

}

void Transform::UpdateLocalMatrix() {
	localMatrix =
		Matrix4::Translation(position) *
		Matrix4(orientation) *
		Matrix4::Scale(scale);

	UpdateGlobalMatrix();
}

Transform& Transform::SetPosition(const Vector3& worldPos) {
	position = worldPos;
	UpdateLocalMatrix();
	return *this;
}

Transform& Transform::SetScale(const Vector3& worldScale) {
	scale = worldScale;
	UpdateLocalMatrix();
	return *this;
}

Transform& Transform::SetOrientation(const Quaternion& worldOrientation) {
	orientation = worldOrientation;
	UpdateLocalMatrix();
	UpdateGlobalOrientation();
	return *this;
}

void Transform::UpdateGlobalMatrix() {
	globalMatrix = parent ? parent->GetGlobalMatrix() * localMatrix : localMatrix;

	for (Transform* child : children) {
		child->UpdateGlobalMatrix();
	}
}

void Transform::UpdateGlobalOrientation() {
	globalOrientation = parent ? parent->GetGlobalOrientation() * orientation : orientation;

	for (Transform* child : children) {
		child->UpdateGlobalOrientation();
	}
}