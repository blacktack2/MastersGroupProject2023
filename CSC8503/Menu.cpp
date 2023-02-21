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

void Menu::Draw(OGLShader* menuShader, OGLTexture* menuTexture, OGLMesh* quad)
{
	for (Button* btn:buttons) {
		btn->Draw(Debug::GREEN);
	}
}

void Menu::Update(float dt) {
	UIObject::Update(dt);
	Vector2 mousePos;
	bool hasMouse = false;
	if (Window::GetMouse()) {
		mousePos = Window::GetMouse()->GetAbsolutePosition();
		hasMouse = true;
	}
	for (Button* btn : buttons) {
		btn->Update(dt);
		if (hasMouse) {
			if (btn->CheckMousePosition(mousePos)) {
				return;
			}	
		}
	}
}

Vector4 Menu::GetDimension()
{
	return Vector4(screenPos, dimension);
}

