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
	initMainMenu();
}

MenuManager::~MenuManager() {
	for (auto menu : menus) {
		delete (menu.second);
	}
	menus.clear();
}

void MenuManager::Draw(OGLShader* menuShader, OGLTexture* menuTexture, OGLMesh* quad)
{
	menus[currentMenu]->Draw(menuShader, menuTexture, quad);
}

void MenuManager::Update(float dt) {
	for (auto menu : menus) {
		(menu.second)->Update(dt);
	}
}

Vector4 MenuManager::GetMenuDimension()
{
	return menus[currentMenu]->GetDimension();
}

Vector4 MenuManager::PixelToScreenSpace(float screenWidth, float screenHeight, Vector4 componentDimension) {
	float x = componentDimension.x - componentDimension.z / 2;
	float y = componentDimension.y - componentDimension.w / 2;

	return Vector4(x / screenWidth * 2 - 1,- ( y / screenHeight * 2 - 1 ), (x + componentDimension.z) / screenWidth * 2 - 1, - ( (y + componentDimension.w) / screenHeight * 2 - 1 ) );
}

void MenuManager::initMainMenu() {
	menus["start"] = new Menu(Vector2(512, 200), Vector2(100, 100));
	menus["start"]->SetRenderObject(new RenderObject(nullptr, nullptr, nullptr, nullptr));
	menus["start"]->AddButton(new Button(500, 100, 100, 30));
}

