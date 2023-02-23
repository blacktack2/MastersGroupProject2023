/**
 * @file   ScreenMain.h
 * @brief  A class for defining the main menu.
 *
 * @author Felix Chiu
 * @date   February 2023
 */
#pragma once
#include "PushdownState.h"
#include "Window.h"
#include "MenuManager.h"
#include "ScreenGame.h"

using namespace NCL;
using namespace CSC8503;

class ScreenMain : public PushdownState {
public:
	ScreenMain(){
		initMenu();
	}
	~ScreenMain(){}

	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		menuManager.Update(dt);
		keyMap.Update();
		renderer.Render();
		if (menuState == ChangeState::Start) {
			*newState = new ScreenGame();
			return PushdownResult::Push;
		}
		if (menuState == ChangeState::Quit) {
			renderer.EnableRenderScene(true);
			renderer.UpdatePipeline();
			return PushdownResult::Pop;
		}
		menuState = ChangeState::None;
		return PushdownResult::NoChange;
	}
	void OnAwake() override {
		menuManager.SetCurrentMenu("main");
		renderer.EnableRenderScene(false);
		renderer.UpdatePipeline();
		Window::GetWindow()->ShowOSPointer(true);
		Window::GetWindow()->LockMouseToWindow(false);
	}
	


	GameTechRenderer& renderer = GameTechRenderer::instance();
	MenuManager& menuManager = MenuManager::instance();
	paintHell::InputKeyMap& keyMap = paintHell::InputKeyMap::instance();

private:
	void initMenu();
	void initQuad();

	enum class ChangeState {
		None,
		OnGoing,
		Start,
		Quit
	};
	ChangeState menuState = ChangeState::None;
};


