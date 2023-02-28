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
	ShaderBase* shader = (ShaderBase*)AssetLibrary::GetShader("menu");

	TextureBase* mainMenuBg = AssetLibrary::GetTexture("menuMain");

	MeshGeometry* quad = (MeshGeometry*)AssetLibrary::GetMesh("quad");
	Menu* menu = new Menu(Vector2(0, 0), Vector2(1, 1));
	menu->SetRenderObject(new RenderObject(nullptr, quad, mainMenuBg, shader));
	menuManager.AddMenu(name, menu);

	//button 1
	TextureBase* tex = AssetLibrary::GetTexture("button1");
	Button* btn = new Button(0.5f, 0.0f, 0.2f, 0.1f);
	btn->SetRenderObject(new RenderObject(nullptr, quad, tex, shader));
	menu->AddButton(btn);
	btn->OnClickCallback = [&]() {
		std::cout << "Start server clicked" << std::endl;
		menuState = ChangeState::StartServer;
	};

	//button 2
	tex = AssetLibrary::GetTexture("button1");
	btn = new Button(0.5f, -0.3f, 0.2f, 0.1f);
	btn->SetRenderObject(new RenderObject(nullptr, quad, tex, shader));
	menu->AddButton(btn);
	btn->OnClickCallback = [&]() {
		std::cout << "Start client clicked" << std::endl;
		menuState = ChangeState::StartClient;
	};

	//button 3
	tex = AssetLibrary::GetTexture("button3");
	btn = new Button(0.5f, 2 * -0.3f, 0.2f, 0.1f);
	btn->SetRenderObject(new RenderObject(nullptr, quad, tex, shader));
	menu->AddButton(btn);
	btn->OnClickCallback = [&]() {
		menuState = ChangeState::Quit;
	};
}