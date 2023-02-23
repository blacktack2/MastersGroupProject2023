/**
 * @file   MenuManager.h
 * @brief  See Menu.h.
 *
 * @author Felix Chiu
 * @date   February 2023
 */
#include "MenuManager.h"
#include "AssetLibrary.h"

using namespace NCL;
using namespace CSC8503;
using namespace Maths;

MenuManager::MenuManager() {
	renderer = &GameTechRenderer::instance();
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

void MenuManager::AddMenu(string name, Menu* menu)
{
	if (!menus[name]) {
		menus[name] = menu;
	}
}

Vector4 MenuManager::PixelToScreenSpace(float screenWidth, float screenHeight, Vector4 componentDimension) {
	float x = componentDimension.x - componentDimension.z / 2;
	float y = componentDimension.y - componentDimension.w / 2;

	return Vector4(x / screenWidth * 2 - 1,- ( y / screenHeight * 2 - 1 ), (x + componentDimension.z) / screenWidth * 2 - 1, - ( (y + componentDimension.w) / screenHeight * 2 - 1 ) );
}




