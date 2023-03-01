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
#include "ScreenOption.h"
#include "ScreenMultiplayerOption.h"

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
		switch (menuState)
		{
		case ChangeState::Start:
			*newState = new ScreenGame();
			return PushdownResult::Push;
		case ChangeState::Multiplayer:
			*newState = new ScreenMultiplayerOption();
			return PushdownResult::Push;
		case ChangeState::Option:
			*newState = new ScreenOption();
			return PushdownResult::Push;
		case ChangeState::Quit:
			return PushdownResult::Pop;
		default:
			menuState = ChangeState::OnGoing;
			return PushdownResult::NoChange;
		}
	}
	void OnAwake() override {
		menuState = ChangeState::None;
		menuManager.SetCurrentMenu(name);
		renderer.EnableOverlayPass("Menu", true);
		renderer.EnableOverlayPass("Debug", false);
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

	enum class ChangeState {
		None,
		OnGoing,
		Start,
		Multiplayer,
		Option,
		Quit
	};
	ChangeState menuState = ChangeState::None;

	string name = "main";
};


