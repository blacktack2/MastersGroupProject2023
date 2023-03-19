/**
 * @file   Menu.cpp
 * @brief  See Menu.h.
 *
 * @author Felix Chiu
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "Menu.h"

using namespace NCL;
using namespace NCL::CSC8503;
using namespace NCL::Maths;

Menu::Menu(Vector2 screenPos, Vector2 dimension, std::shared_ptr<TextureBase> texture) : UIObject(texture) {
	this->screenPos = screenPos;
	this->dimension = dimension;
}

Menu::~Menu() {
	buttons.clear();
}

void Menu::Update(float dt) {
	for (auto& btn : buttons) {
		btn->Update(dt);
	}
}

Vector4 Menu::GetDimension() const {
	return Vector4(screenPos, dimension);
}

Button& Menu::AddButton(float x, float y, float width, float height, std::shared_ptr<TextureBase> texture, Button::overlap_func onclick, Button::overlap_func onselect, Button::overlap_func onpress) {
	buttons.push_back(std::move(std::make_unique<Button>(x, y, width, height, texture, onclick, onselect, onpress)));
	Button& button = *buttons.back();
	AddChild(button);
	return button;
}
