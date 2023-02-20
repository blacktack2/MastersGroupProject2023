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
	buttons.empty();
}

void Menu::Draw()
{
	for (Button* btn:buttons) {
		btn->Draw(Debug::GREEN);
	}
}

void Menu::Update(float dt) {
	
}

