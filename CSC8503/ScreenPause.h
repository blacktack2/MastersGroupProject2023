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
		keyMap.Update();
		if (keyMap.GetButton(InputType::ESC)) {
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;
	}
	void OnAwake() override {
		std::cout << "Press ESC to unpause game\n";
		Window::GetWindow()->ShowOSPointer(false);
		Window::GetWindow()->LockMouseToWindow(true);
		renderer.EnableRenderScene(true);
		renderer.EnableOverlayPass("Menu", false);
		renderer.UpdatePipeline();
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
		Quit
	};
	ChangeState menuState = ChangeState::None;
};
