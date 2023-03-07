/**
 * @file   ScreenMultiplayerOption.cpp
 * @brief  See ScreenMultiplayerOption.h .
 *
 * @author Felix Chiu
 * @date   February 2023
 */
#pragma once
#include "ScreenMultiplayerOption.h"
#include "AssetLibrary.h"

using namespace NCL;
using namespace CSC8503;

void ScreenMultiplayerOption::initMenu() {
	TextureBase* mainMenuBg = AssetLibrary::instance().GetTexture("menuMain").get();

	Menu* menu = new Menu(Vector2(0, 0), Vector2(1, 1));
	menu->SetRenderObject(new MenuRenderObject(mainMenuBg));
	menuManager.AddMenu(name, menu);

	//button 1
	TextureBase* tex = AssetLibrary::instance().GetTexture("button1").get();
	Button* btn = new Button(0.5f, 0.0f, 0.2f, 0.1f);
	btn->SetRenderObject(new MenuRenderObject(tex));
	menu->AddButton(btn);
	btn->OnClickCallback = [&]() {
		std::cout << "Start server clicked" << std::endl;
		menuState = ChangeState::StartServer;
	};

	//button 2
	tex = AssetLibrary::instance().GetTexture("button1").get();
	btn = new Button(0.5f, -0.3f, 0.2f, 0.1f);
	btn->SetRenderObject(new MenuRenderObject(tex));
	menu->AddButton(btn);
	btn->OnClickCallback = [&]() {
		std::cout << "Start client clicked" << std::endl;
		menuState = ChangeState::StartClient;
	};

	//button 3
	tex = AssetLibrary::instance().GetTexture("button3").get();
	btn = new Button(0.5f, 2 * -0.3f, 0.2f, 0.1f);
	btn->SetRenderObject(new MenuRenderObject(tex));
	menu->AddButton(btn);
	btn->OnClickCallback = [&]() {
		menuState = ChangeState::Quit;
	};
}