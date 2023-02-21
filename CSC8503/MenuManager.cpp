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
	initQuad();
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

void MenuManager::initQuad()
{
	quad = new OGLMesh();
	quad->SetVertexPositions({
		Vector3(-1, 1, -1),
		Vector3(-1, -1, -1),
		Vector3(1, -1, -1),
		Vector3(1, 1, -1),
		});
	quad->SetVertexTextureCoords({
		Vector2(0, 0),
		Vector2(0, 1),
		Vector2(1, 1),
		Vector2(1, 0),
		});
	quad->SetVertexIndices({ 0, 1, 2, 2, 3, 0 });
	quad->SetVertexColours({
		Vector4(1, 1, 1, 1),
		Vector4(1, 1, 1, 1),
		Vector4(1, 1, 1, 1),
		Vector4(1, 1, 1, 1),
		});
}

void MenuManager::initMainMenu() {
	OGLShader* shader = (OGLShader*)renderer->LoadShader("menuVertex.vert", "menuFragment.frag");
	TextureBase* startMenuBg = renderer->LoadTexture("defaultstart.jpg");
	AssetLibrary::AddTexture("startMenuBg", startMenuBg);
	menus["start"] = new Menu(Vector2(renderer->GetWidth() / 2, renderer->GetHeight() / 2), Vector2(renderer->GetWidth()/2, renderer->GetHeight()/2));
	menus["start"]->SetRenderObject(new RenderObject(nullptr, quad, startMenuBg, shader));
	
	TextureBase* quitBtn = renderer->LoadTexture("button3.jpg");
	float scale = 0.1f;
	Button* btn = new Button(500, 100, 790 * scale, 180 * scale);
	btn->SetRenderObject(new RenderObject(nullptr, quad, quitBtn, shader));
	menus["start"]->AddButton(btn);
}

