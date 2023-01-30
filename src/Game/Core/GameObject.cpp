/**
 * @file   GameObject.cpp
 * @brief  See GameObject.h
 *
 * @author Harry Brettell
 * @date   January 2023
*/
#include "GameObject.h"
#include "Transform.h"
#include <iostream>

using namespace paintHell::core;

GameObject::GameObject() : transform(this) {
	tag = "";
}

GameObject::GameObject(std::string name) : transform(this) {
	tag = name;
}

GameObject::~GameObject() {
}