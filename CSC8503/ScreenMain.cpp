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
	initQuad();
	ShaderBase* shader = (ShaderBase*)renderer.LoadShader("menuVertex.vert", "menuFragment.frag");
	TextureBase* mainMenuBg = renderer.LoadTexture("defaultMain.jpg");
	AssetLibrary::AddTexture("mainMenuBg", mainMenuBg);
	MeshGeometry* quad = (MeshGeometry*)AssetLibrary::GetMesh("quad");
	Menu* main = new Menu(Vector2(0,0), Vector2(1,1));
	main->SetRenderObject(new RenderObject(nullptr, quad, mainMenuBg, shader));
	menuManager.AddMenu("main",main);

	TextureBase* startTex = renderer.LoadTexture("button1.jpg");
	Button* startBtn = new Button(0.7f, -0.3f, 0.2f, 0.1f);
	startBtn->SetRenderObject(new RenderObject(nullptr, quad, startTex, shader));
	main->AddButton(startBtn);

	startBtn->OnClickCallback = [&]() {
		std::cout << "Start btn clicked" << std::endl;
		menuState = ChangeState::Start;
	};

	TextureBase* quitTex = renderer.LoadTexture("button3.jpg");
	Button* quitBtn = new Button(0.7f, -0.6f, 0.2f, 0.1f);
	quitBtn->SetRenderObject(new RenderObject(nullptr, quad, quitTex, shader));
	main->AddButton(quitBtn);

	quitBtn->OnClickCallback = [&]() {
		std::cout << "quit btn clicked" << std::endl;
		menuState = ChangeState::Quit;
	};
}
void ScreenMain::initQuad()
{
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


