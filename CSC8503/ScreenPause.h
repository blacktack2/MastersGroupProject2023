/**
 * @file   ScreenPause.h
 * @brief  A Pushdown automata state for Pausing the game.
 *
 * @author Felix Chiu
 * @date   February 2023
 */
#pragma once
#include "PushdownState.h"
#include "PushDownStateChangeManager.h"

class ScreenPause : public PushdownState {
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		menuManager.Update(dt);
		keyMap.Update();
		renderer.Render();
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
		menuManager.SetCurrentMenu("pause");
		renderer.EnableRenderScene(false);
		renderer.UpdatePipeline();
		stateChangeManager.SetChangeState();
	}

	PushDownStateChangeManager& stateChangeManager = PushDownStateChangeManager::instance();
	GameTechRenderer& renderer = GameTechRenderer::instance();
	MenuManager& menuManager = MenuManager::instance();
	paintHell::InputKeyMap& keyMap = paintHell::InputKeyMap::instance();
};
