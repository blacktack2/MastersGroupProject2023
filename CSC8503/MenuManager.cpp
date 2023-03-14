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

MenuManager::MenuManager() : renderer(GameTechRenderer::instance()) {
}

MenuManager::~MenuManager() {
	menus.clear();
}

void MenuManager::Update(float dt) {
	menus[currentMenu]->Update(dt);
	/*
	for (auto& menu : menus) {
		menu.second->Update(dt);
	}
	*/
}

Vector4 MenuManager::GetMenuDimension() {
	return menus[currentMenu]->GetDimension();
}

Menu& MenuManager::AddMenu(const std::string& name, const Vector2& position, const Vector2& size, std::shared_ptr<TextureBase> texture) {
	return *menus.emplace(name, std::move(std::make_unique<Menu>(position, size, texture))).first->second;
}

Vector4 MenuManager::PixelToScreenSpace(float screenWidth, float screenHeight, Vector4 componentDimension) {
	float x = componentDimension.x - componentDimension.z / 2;
	float y = componentDimension.y - componentDimension.w / 2;

	return Vector4(x / screenWidth * 2 - 1,- ( y / screenHeight * 2 - 1 ), (x + componentDimension.z) / screenWidth * 2 - 1, - ( (y + componentDimension.w) / screenHeight * 2 - 1 ) );
}


void MenuManager::RemoveAndEraseMenu(const std::string& name) {
	if (menus.contains(name))
	{
		menus.erase(name);
	}
}