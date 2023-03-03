/**
 * @file   ScreenMultiplayer.h
 * @brief  A Pushdown automata state for running a multiplayer game.
 *
 * @author Felix Chiu
 * @date   February 2023
 */
#pragma once
#include "PushdownState.h"
#include "Window.h"
#include "MenuManager.h"
#include "ScreenMultiplayer.h"
#include "NetworkedGame.h"
#include "ScreenPause.h"

using namespace NCL;
using namespace CSC8503;

class ScreenMultiplayer : public PushdownState {
public:

	ScreenMultiplayer(bool isServer = true) {
		game = new NetworkedGame(isServer);
	}
	~ScreenMultiplayer() {
		delete game;
	}

	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		keyMap.Update();
		if (keyMap.GetButton(InputType::Pause)) {
			*newState = new ScreenPause(game);
			return PushdownResult::Push;
		}
		if (game->IsQuit()) {
			menuState = ChangeState::Quit;
		}
		else {
			game->UpdateGame(dt);
		}
		if (menuState == ChangeState::Quit) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
			renderer.EnableOverlayPass("Menu", true);
			renderer.UpdatePipeline();
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;
	}
	void OnAwake() override {
		menuState = ChangeState::None;
		Window::GetWindow()->ShowOSPointer(false);
		Window::GetWindow()->LockMouseToWindow(true);
		renderer.EnableRenderScene(true);
		renderer.EnableOverlayPass("Menu", false);
		renderer.EnableOverlayPass("Debug", true);
		renderer.UpdatePipeline();
	}

private:
	enum class ChangeState {
		None,
		OnGoing,
		Start,
		Quit
	};

	
	GameStateManager* gameStateManager = &GameStateManager::instance();
	GameTechRenderer& renderer = GameTechRenderer::instance();
	MenuManager& menuManager = MenuManager::instance();
	paintHell::InputKeyMap& keyMap = paintHell::InputKeyMap::instance();
	NetworkedGame* game;

	ChangeState menuState = ChangeState::None;
};
