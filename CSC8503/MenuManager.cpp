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
	//initPauseMenu();
}

MenuManager::~MenuManager() {
	for (auto menu : menus) {
		delete (menu.second);
	}
	menus.clear();
}

void MenuManager::Draw(OGLShader* menuShader, OGLTexture* menuTexture, OGLMesh* quad){
	menus["main"]->Draw(menuShader, menuTexture, quad);
	//menus["pause"]->Draw(menuShader, menuTexture, quad);
}

void MenuManager::Update(float dt) {
	for (auto menu : menus) {
		(menu.second)->Update(dt);
	}
}

Vector4 MenuManager::GetMenuDimension(){
	return menus[currentMenu]->GetDimension();
}

Vector4 MenuManager::PixelToScreenSpace(float screenWidth, float screenHeight, Vector4 componentDimension) {
	float x = componentDimension.x - componentDimension.z / 2;
	float y = componentDimension.y - componentDimension.w / 2;

	return Vector4(x / screenWidth * 2 - 1,- ( y / screenHeight * 2 - 1 ), (x + componentDimension.z) / screenWidth * 2 - 1, - ( (y + componentDimension.w) / screenHeight * 2 - 1 ) );
}

void MenuManager::initQuad(){
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
	TextureBase* mainMenuBg = renderer->LoadTexture("defaultMain.jpg");
	AssetLibrary::AddTexture("mainMenuBg", mainMenuBg);
	currentMenu = "main";
	menus[currentMenu] = new Menu(Vector2(renderer->GetWidth() / 2, renderer->GetHeight() / 2), Vector2(renderer->GetWidth()/2, renderer->GetHeight()/2));
	menus[currentMenu]->SetRenderObject(new RenderObject(nullptr, quad, mainMenuBg, shader));
	
	int num = 3;
	for (int i = 0; i < num; i++) {
		char path[50] = { 0 };
		sprintf_s(path, "button%d.jpg", i + 1);

		float scale = 0.2f;
		Button* btn = new Button(975, 350 + i * 100, 790 * scale, 180 * scale);
		TextureBase* quitBtn = renderer->LoadTexture(path);

		btn->SetRenderObject(new RenderObject(nullptr, quad, quitBtn, shader));
		menus[currentMenu]->AddButton(btn);

		btn->OnClickCallback = [&]() {
			std::cout << "quit btn clicked" << std::endl;
		};
	}
}

void MenuManager::initPauseMenu() {
	OGLShader* shader = (OGLShader*)renderer->LoadShader("menuVertex.vert", "menuFragment.frag");
	TextureBase* pauseMenuBg = renderer->LoadTexture("defaultpause.jpg");
	AssetLibrary::AddTexture("pauseMenuBg", pauseMenuBg);
	currentMenu = "pause";
	menus[currentMenu] = new Menu(Vector2(renderer->GetWidth() / 2, renderer->GetHeight() / 2), Vector2(renderer->GetWidth() / 7, renderer->GetHeight() / 3));
	menus[currentMenu]->SetRenderObject(new RenderObject(nullptr, quad, pauseMenuBg, shader));

	int num = 4;
	for (int i = 0; i < num; i++) {
		char path[50] = { 0 };
		sprintf_s(path, "button%d.jpg", i + 4);

		float scale = 0.2f;
		Button* btn = new Button(630, 210 + i * 90, 480 * scale, 120 * scale);
		TextureBase* quitBtn = renderer->LoadTexture(path);

		btn->SetRenderObject(new RenderObject(nullptr, quad, quitBtn, shader));
		menus[currentMenu]->AddButton(btn);

		btn->OnClickCallback = [&]() {
			std::cout << "quit btn clicked" << std::endl;
		};
	}
}

