/**
 * @file   ScreenPause.h
 * @brief  A Pushdown automata state for Pausing the game.
 *
 * @author Felix Chiu
 * @date   February 2023
 */
#pragma once
#include "PushdownState.h"
#include "Window.h"
#include "MenuManager.h"

using namespace NCL;
using namespace CSC8503;

class ScreenPause : public PushdownState {
public:
	ScreenPause() {
		initMenu();
	}
	~ScreenPause() {}
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		menuManager.Update(dt);
		keyMap.Update();
		renderer.Render();
		if (menuState == ChangeState::Resume) {
			renderer.EnableOverlayPass("Debug", true);
			return PushdownResult::Pop;
		}
		if (menuState == ChangeState::Quit) {
			renderer.EnableOverlayPass("Debug", true);
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;
	}
	void OnAwake() override {
		menuState = ChangeState::None;
		menuManager.SetCurrentMenu(name);
		renderer.EnableOverlayPass("Menu", true);
		renderer.EnableOverlayPass("Debug", false);
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
		Resume,
		Option,
		Quit
	};
	ChangeState menuState = ChangeState::None;

	string name = "pause";
};
