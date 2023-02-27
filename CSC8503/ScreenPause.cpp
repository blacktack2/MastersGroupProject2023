/**
 * @file   ScreenPause.cpp
 * @brief  See ScreenPause.h .
 *
 * @author Felix Chiu
 * @date   February 2023
 */
#pragma once
#include "ScreenPause.h"

#include "AssetLibrary.h"
#include "MenuRenderObject.h"

using namespace NCL;
using namespace CSC8503;

void ScreenPause::initMenu() {
	ShaderBase* shader = (ShaderBase*)AssetLibrary::GetShader("menu");

	TextureBase* texture = renderer.LoadTexture("defaultpause.jpg");
	AssetLibrary::AddTexture("pauseMenuBg", texture);

	MeshGeometry* quad = (MeshGeometry*)AssetLibrary::GetMesh("quad");
	Menu* menu = new Menu(Vector2(0, 0), Vector2(0.3, 0.7));
	menu->SetRenderObject(new MenuRenderObject(texture));
	menuManager.AddMenu(name, menu);

	//Load button
	vector<Button*> buttons;
	int num = 4;
	for (int i = 0; i < num; i++) {
		char path[50] = { 0 };
		sprintf_s(path, "button%d.jpg", i + 4);

		Button* btn = new Button(0, 0.45f + i * -0.3f, 0.16f, 0.08f, Vector4(0, 0, 0, 1));
		TextureBase* quitBtn = renderer.LoadTexture(path);

		btn->SetRenderObject(new MenuRenderObject(quitBtn));
		menu->AddButton(btn);
		buttons.push_back(btn);
	}
	buttons[0]->OnClickCallback = [&]() {
		std::cout << "1 btn clicked" << std::endl;
		menuState = ChangeState::Resume;
	};
	buttons[1]->OnClickCallback = [&]() {
		std::cout << "2 btn clicked" << std::endl;
	};
	buttons[2]->OnClickCallback = [&]() {
		std::cout << "3 btn clicked" << std::endl;
	};
	buttons[3]->OnClickCallback = [&]() {
		std::cout << "4 btn clicked" << std::endl;
		gameStateManager->SetGameState(GameState::Quit);
		menuState = ChangeState::Quit;
	};
}