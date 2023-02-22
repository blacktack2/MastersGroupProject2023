/**
 * @file   ScreenGame.h
 * @brief  A Pushdown automata state for running the game.
 *
 * @author Felix Chiu
 * @date   February 2023
 */
#pragma once
#include "PushdownState.h"
#include "PushDownStateChangeManager.h"

class ScreenGame : public PushdownState {
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		game->UpdateGame(dt);
		if (game->IsQuit()) {
			std::cout << "Returning to main menu!\n";
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
			renderer.EnableOverlayPass("Menu", true);
			renderer.UpdatePipeline();
			delete game;
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
		stateChangeManager.SetChangeState();
		game = new NetworkedGame();
	}
	NetworkedGame* game;
	PushDownStateChangeManager& stateChangeManager = PushDownStateChangeManager::instance();
	GameTechRenderer& renderer = GameTechRenderer::instance();
	MenuManager& menuManager = MenuManager::instance();
	paintHell::InputKeyMap& keyMap = paintHell::InputKeyMap::instance();
};
