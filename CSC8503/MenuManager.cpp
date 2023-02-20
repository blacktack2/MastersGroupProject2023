/**
 * @file   MenuManager.h
 * @brief  See Menu.h.
 *
 * @author Felix Chiu
 * @date   February 2023
 */
#include "MenuManager.h"

using namespace NCL;
using namespace CSC8503;
using namespace Maths;

MenuManager::MenuManager() {
	menus["start"] = Menu(Vector2(10,10),Vector2(10,10));
}

MenuManager::~MenuManager() {
}

void MenuManager::Draw()
{
	menus["start"].Draw();
}

void MenuManager::Update(float dt) {
	
}

