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
	initQuad();
	ShaderBase* shader = (ShaderBase*)AssetLibrary::GetShader("menu");

	TextureBase* mainMenuBg = renderer.LoadTexture("defaultMain.jpg");
	AssetLibrary::AddTexture("mainMenuBg", mainMenuBg);

	MeshGeometry* quad = (MeshGeometry*)AssetLibrary::GetMesh("quad");
	Menu* menu = new Menu(Vector2(0,0), Vector2(1,1));
	menu->SetRenderObject(new MenuRenderObject(mainMenuBg));
	menuManager.AddMenu(name, menu);

	//button 1
	TextureBase* startTex = renderer.LoadTexture("button1.jpg");
	Button* startBtn = new Button(0.5f, 0.0f, 0.2f, 0.1f);
	startBtn->SetRenderObject(new MenuRenderObject(startTex));
	menu->AddButton(startBtn);
	startBtn->OnClickCallback = [&]() {
		std::cout << "Start btn clicked" << std::endl;
		menuState = ChangeState::Start;
	};

	//button 2
	TextureBase* optionTex = renderer.LoadTexture("button2.jpg");
	Button* optionBtn = new Button(0.5f, -0.3f, 0.2f, 0.1f);
	optionBtn->SetRenderObject(new MenuRenderObject(optionTex));
	menu->AddButton(optionBtn);
	optionBtn->OnClickCallback = [&]() {
		std::cout << "Option btn clicked" << std::endl;
		menuState = ChangeState::Option;
	};

	//button 3
	TextureBase* quitTex = renderer.LoadTexture("button3.jpg");
	Button* quitBtn = new Button(0.5f, 2 * -0.3f, 0.2f, 0.1f);
	quitBtn->SetRenderObject(new MenuRenderObject(quitTex));
	menu->AddButton(quitBtn);
	quitBtn->OnClickCallback = [&]() {
		std::cout << "Quit btn clicked" << std::endl;
		menuState = ChangeState::Quit;
	};
}
void ScreenMain::initQuad() {
	ShaderBase* shader = (ShaderBase*)renderer.LoadShader("menuVertex.vert", "menuFragment.frag");
	AssetLibrary::AddShader("menu", shader);

	OGLMesh* quad = new OGLMesh();
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
	AssetLibrary::AddMesh("quad", quad);
}


