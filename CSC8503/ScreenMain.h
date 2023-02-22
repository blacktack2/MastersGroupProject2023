/**
 * @file   IntroState.h
 * @brief  A class which lets other UI related class inherits from.
 *
 * @author Felix Chiu
 * @date   February 2023
 */
#pragma once
#include "PushdownState.h"
#include "PushDownStateChangeManager.h"

#include "ScreenGame.h"
#include "AssetLibrary.h"

class ScreenMain : public PushdownState {
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		menuManager.Update(dt);
		keyMap.Update();
		renderer.Render();
		if (stateChangeManager.GetChangeState() == ChangeState::Option1) {
			*newState = new ScreenGame();
			return PushdownResult::Push;
		}
		if (stateChangeManager.GetChangeState() == ChangeState::Quit) {
			renderer.EnableRenderScene(true);
			renderer.UpdatePipeline();
			stateChangeManager.SetChangeState();
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;
	}
	void OnAwake() override {
		std::cout << "Welcome!\n";
		std::cout << "Press space to begin, or escape to quit!\n";
		menuManager.SetCurrentMenu("main");
		renderer.EnableRenderScene(false);
		renderer.UpdatePipeline();
		stateChangeManager.SetChangeState();
		Window::GetWindow()->ShowOSPointer(true);
		Window::GetWindow()->LockMouseToWindow(false);
		initMainMenu();
	}

	PushDownStateChangeManager& stateChangeManager = PushDownStateChangeManager::instance();
	GameTechRenderer& renderer = GameTechRenderer::instance();
	MenuManager& menuManager = MenuManager::instance();
	paintHell::InputKeyMap& keyMap = paintHell::InputKeyMap::instance();

private:
	void initMainMenu() {
		initQuad();
		OGLShader* shader = (OGLShader*)renderer.LoadShader("menuVertex.vert", "menuFragment.frag");
		TextureBase* mainMenuBg = renderer.LoadTexture("defaultMain.jpg");
		AssetLibrary::AddTexture("mainMenuBg", mainMenuBg);
		OGLMesh* quad = (OGLMesh*)AssetLibrary::GetMesh("quad");
		Menu* main = new Menu(Vector2(0,0), Vector2(1,1));
		main->SetRenderObject(new RenderObject(nullptr, quad, mainMenuBg, shader));
		menuManager.AddMenu("main",main);

		TextureBase* startTex = renderer.LoadTexture("button1.jpg");
		Button* startBtn = new Button(0.7f, -0.3f, 0.2f, 0.1f);
		startBtn->SetRenderObject(new RenderObject(nullptr, quad, startTex, shader));
		main->AddButton(startBtn);

		startBtn->OnClickCallback = [&]() {
			std::cout << "Start btn clicked" << std::endl;
			PushDownStateChangeManager& stateChangeManager = PushDownStateChangeManager::instance();
			stateChangeManager.SetChangeState(ChangeState::Option1);
		};

		TextureBase* quitTex = renderer.LoadTexture("button3.jpg");
		Button* quitBtn = new Button(0.7f, -0.6f, 0.2f, 0.1f);
		quitBtn->SetRenderObject(new RenderObject(nullptr, quad, quitTex, shader));
		main->AddButton(quitBtn);

		quitBtn->OnClickCallback = [&]() {
			std::cout << "quit btn clicked" << std::endl;
			PushDownStateChangeManager& stateChangeManager = PushDownStateChangeManager::instance();
			stateChangeManager.SetChangeState(ChangeState::Quit);
		};
	}
	void initQuad()
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
};


