/**
 * @file   ScreenMultiplayerOption.h
 * @brief  A Pushdown automata state for multiplayer game options.
 *
 * @author Felix Chiu
 * @date   February 2023
 */
#pragma once
#include "PushdownState.h"
#include "Window.h"
#include "MenuManager.h"
#include "ScreenMultiplayer.h"

using namespace NCL;
using namespace CSC8503;

class ScreenMultiplayerOption : public PushdownState {
public:
	ScreenMultiplayerOption() {
		initMenu();
	}
	~ScreenMultiplayerOption() {}
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		menuManager.Update(dt);
		keyMap.Update();
		renderer.Render();
		if (menuState == ChangeState::StartServer) {
			*newState = new ScreenMultiplayer(true);
			return PushdownResult::Push;
		}
		if (menuState == ChangeState::StartClient) {
			*newState = new ScreenMultiplayer(false);
			return PushdownResult::Push;
		}
		if (menuState == ChangeState::Quit) {
			return PushdownResult::Pop;
		}
		menuState = ChangeState::OnGoing;
		return PushdownResult::NoChange;
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
		StartServer,
		StartClient,
		Quit
	};
	ChangeState menuState = ChangeState::None;

	string name = "ScreenMultiplayerOption";
};
