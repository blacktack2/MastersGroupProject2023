#include "Transform.h"
#include <vector>

using namespace NCL;
using namespace CSC8503;

Transform::Transform(GameObject* gameObject){
	this->gameObject = gameObject;
	this->parent = NULL;
	scale = Vector3(1, 1, 1);
}

Transform::Transform(GameObject* gameObject, Transform& transform) {
	this->gameObject = gameObject;
	this->parent = transform.parent;
	scale = Vector3(transform.scale);
	position = Vector3(transform.position);
	orientation = Quaternion(transform.orientation);
	UpdateLocalMatrix();
}

Transform::~Transform()	{
	if (parent)
	{
		parent->RemoveChild(this);
	}
	for (Transform* child : children)
	{
		child->SetParent(nullptr);
	}
	children.clear();
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

void Transform::AddChild(Transform* child) {
	if (child)
	{
		children.emplace_back(child);
		child->SetParent(this);
	}
}

void Transform::RemoveChild(Transform* child) {
	children.erase(std::remove(children.begin(), children.end(), child), children.end());
}