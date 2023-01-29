#include "GameObject.h"
#include "Transform.h"
#include <iostream>

using namespace paintHell::core;

GameObject::GameObject() {
	transform = Transform(this);
	tag = "";
}

GameObject::GameObject(std::string name) {
	transform = Transform(this);
	tag = name;
}

GameObject::~GameObject() {
}