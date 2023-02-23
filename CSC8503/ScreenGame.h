/**
 * @file   ScreenGame.h
 * @brief  A Pushdown automata state for running the game.
 *
 * @author Felix Chiu
 * @date   February 2023
 */
#pragma once
#include "PushdownState.h"
#include "Window.h"
#include "MenuManager.h"
#include "NetworkedGame.h"
#include "ScreenPause.h"

using namespace NCL;
using namespace CSC8503;

class ScreenGame : public PushdownState {
public:
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		keyMap.Update();
		if (keyMap.GetButton(InputType::ESC)) {
			std::cout << "Pausing\n";
			*newState = new ScreenPause();
			return PushdownResult::Push;
		}
		game->UpdateGame(dt);
		if (game->IsQuit()) {
			menuState = ChangeState::Quit;
		}
		if (menuState == ChangeState::Quit) {
			std::cout << "Returning to main menu!\n";
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
			renderer.EnableOverlayPass("Menu", true);
			renderer.UpdatePipeline();
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;
	}
	void OnAwake() override {
		Window::GetWindow()->ShowOSPointer(false);
		Window::GetWindow()->LockMouseToWindow(true);
		renderer.EnableRenderScene(true);
		renderer.EnableOverlayPass("Menu", false);
		renderer.UpdatePipeline();
		game = new NetworkedGame();
	}

	~ScreenGame() {
		delete game;
	}


	NetworkedGame* game;
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
