/**
 * @file   ScreenMain.h
 * @brief  A class for defining the main menu.
 *
 * @author Felix Chiu
 * @date   February 2023
 */
#pragma once
#include "ScreenMain.h"

#include "AssetLibrary.h"
#include "MenuRenderObject.h"

using namespace NCL;
using namespace CSC8503;

void ScreenMain::initMenu() {
	ShaderBase* shader = (ShaderBase*)AssetLibrary::GetShader("menu");

	TextureBase* mainMenuBg = AssetLibrary::GetTexture("menuMain");

	MeshGeometry* quad = (MeshGeometry*)AssetLibrary::GetMesh("quad");
	Menu* menu = new Menu(Vector2(0,0), Vector2(1,1));
	menu->SetRenderObject(new MenuRenderObject(mainMenuBg));
	menuManager.AddMenu(name, menu);

	//button 1
	TextureBase* startTex = AssetLibrary::GetTexture("button1");
	Button* startBtn = new Button(0.5f, 0.0f, 0.2f, 0.1f);
	startBtn->SetRenderObject(new MenuRenderObject(startTex));
	menu->AddButton(startBtn);
	startBtn->OnClickCallback = [&]() {
		std::cout << "Start btn clicked" << std::endl;
		menuState = ChangeState::Start;
	};

	//button 2
	TextureBase* optionTex = AssetLibrary::GetTexture("button2");
	Button* optionBtn = new Button(0.5f, -0.3f, 0.2f, 0.1f);
	optionBtn->SetRenderObject(new MenuRenderObject(optionTex));
	menu->AddButton(optionBtn);
	optionBtn->OnClickCallback = [&]() {
		std::cout << "Option btn clicked" << std::endl;
		menuState = ChangeState::Option;
	};

	//button 3
	TextureBase* quitTex = AssetLibrary::GetTexture("button3");
	Button* quitBtn = new Button(0.5f, 2 * -0.3f, 0.2f, 0.1f);
	quitBtn->SetRenderObject(new MenuRenderObject(quitTex));
	menu->AddButton(quitBtn);
	quitBtn->OnClickCallback = [&]() {
		std::cout << "Quit btn clicked" << std::endl;
		menuState = ChangeState::Quit;
	};
}