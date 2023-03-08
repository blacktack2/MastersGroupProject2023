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
#include "GameStateManager.h"
#include "NetworkedGame.h"

using namespace NCL;
using namespace CSC8503;

class ScreenPause : public PushdownState {
public:
	ScreenPause(std::optional<std::reference_wrapper<TutorialGame>> game = std::optional<std::reference_wrapper<TutorialGame>>());
	~ScreenPause() = default;
	PushdownResult OnUpdate(float dt, PushdownState** newState) override;
	void OnAwake() override;
private:
	enum class ChangeState {
		None,
		OnGoing,
		Resume,
		Option,
		Quit
	};

	void InitMenu();

	std::optional<std::reference_wrapper<TutorialGame>> game;

	GameStateManager& gameStateManager = GameStateManager::instance();
	GameTechRenderer& renderer = GameTechRenderer::instance();
	MenuManager& menuManager = MenuManager::instance();
	paintHell::InputKeyMap& keyMap = paintHell::InputKeyMap::instance();

	ChangeState menuState = ChangeState::None;

	std::string NAME = "pause";
};
