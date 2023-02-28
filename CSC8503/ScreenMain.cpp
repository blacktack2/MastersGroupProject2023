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

using namespace NCL;
using namespace CSC8503;

void ScreenMain::initMenu() {
	ShaderBase* shader = (ShaderBase*)AssetLibrary::GetShader("menu");

	TextureBase* mainMenuBg = AssetLibrary::GetTexture("menuMain");

	MeshGeometry* quad = (MeshGeometry*)AssetLibrary::GetMesh("quad");
	Menu* menu = new Menu(Vector2(0,0), Vector2(1,1));
	menu->SetRenderObject(new RenderObject(nullptr, quad, mainMenuBg, shader));
	menuManager.AddMenu(name, menu);

	//button 1
	TextureBase* tex = AssetLibrary::GetTexture("button1");
	Button* btn = new Button(0.5f, 0.3f, 0.2f, 0.1f);
	btn->SetRenderObject(new RenderObject(nullptr, quad, tex, shader));
	menu->AddButton(btn);
	btn->OnClickCallback = [&]() {
		std::cout << "Start btn clicked" << std::endl;
		menuState = ChangeState::Start;
	};

	//button 2
	tex = AssetLibrary::GetTexture("button1");
	btn = new Button(0.5f, 0.0f, 0.2f, 0.1f);
	btn->SetRenderObject(new RenderObject(nullptr, quad, tex, shader));
	menu->AddButton(btn);
	btn->OnClickCallback = [&]() {
		std::cout << "Multiplayer btn clicked" << std::endl;
		menuState = ChangeState::Multiplayer;
	};

	//button 3
	TextureBase* optionTex = AssetLibrary::GetTexture("button2");
	Button* optionBtn = new Button(0.5f, -0.3f, 0.2f, 0.1f);
	optionBtn->SetRenderObject(new RenderObject(nullptr, quad, optionTex, shader));
	menu->AddButton(optionBtn);
	optionBtn->OnClickCallback = [&]() {
		std::cout << "Option btn clicked" << std::endl;
		menuState = ChangeState::Option;
	};

	//button 4
	TextureBase* quitTex = AssetLibrary::GetTexture("button3");
	Button* quitBtn = new Button(0.5f, 2 * -0.3f, 0.2f, 0.1f);
	quitBtn->SetRenderObject(new RenderObject(nullptr, quad, quitTex, shader));
	menu->AddButton(quitBtn);
	quitBtn->OnClickCallback = [&]() {
		std::cout << "Quit btn clicked" << std::endl;
		menuState = ChangeState::Quit;
	};
}


