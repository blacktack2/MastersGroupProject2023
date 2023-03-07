/**
 * @file   ScreenOption.cpp
 * @brief  See ScreenOption.h .
 *
 * @author Felix Chiu
 * @date   February 2023
 */
#pragma once
#include "ScreenOption.h"

#include "AssetLibrary.h"
#include "MenuRenderObject.h"

using namespace NCL;
using namespace CSC8503;

void ScreenOption::initMenu() {
	std::cout << "init Option" << std::endl;

	TextureBase* texture = AssetLibrary::GetTexture("menuOption").get();

	Menu* menu = new Menu(Vector2(0, 0), Vector2(1.0f, 1.0f ));
	menu->SetRenderObject(new MenuRenderObject(texture));
	menuManager.AddMenu(name, menu);

	//Load button
	std::vector<Button*> buttons;
	int num = 4;
	for (int i = 0; i < num; i++) {
		char name[8] = { 0 };
		sprintf_s(name, "button%d", i + 4);

		Button* btn = new Button(0, 0.45f + i * -0.3f, 0.16f, 0.08f);
		TextureBase* tex = AssetLibrary::GetTexture(name).get();

		btn->SetRenderObject(new MenuRenderObject(tex));
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
		menuState = ChangeState::Quit;
	};
}