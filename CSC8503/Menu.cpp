/**
 * @file   Menu.cpp
 * @brief  See Menu.h.
 *
 * @author Felix Chiu
 * @date   February 2023
 */
#include "Menu.h"

using namespace NCL;
using namespace CSC8503;
using namespace Maths;

Menu::Menu(Vector2 screenPos, Vector2 dimension) {
	this->screenPos = screenPos;
	this->dimension = dimension;
}

Menu::~Menu() {
	for (Button* btn : buttons) {
		delete btn;
	}
	buttons.clear();
}

void Menu::Draw(OGLShader* menuShader, OGLTexture* menuTexture, OGLMesh* quad){
	for (Button* btn:buttons) {
		btn->Draw(Debug::GREEN);
	}
}

void Menu::Update(float dt) {
	UIObject::Update(dt);
	for (Button* btn : buttons) {
		btn->Update(dt);
	}
}

Vector4 Menu::GetDimension(){
	return Vector4(screenPos, dimension);
}

