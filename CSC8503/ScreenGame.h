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
#include "GameStateManager.h"
#include "NetworkedGame.h"
#include "ScreenPause.h"

using namespace NCL;
using namespace NCL::CSC8503;

class ScreenGame : public PushdownState {
public:
	ScreenGame();
	~ScreenGame() = default;

	PushdownResult OnUpdate(float dt, PushdownState** newState) override;

	void OnAwake() override;

private:
	enum class ChangeState {
		None,
		OnGoing,
		Start,
		Quit
	};

	float time = 0;
	int count = 0;
	
	GameStateManager& gameStateManager = GameStateManager::instance();
	GameTechRenderer& renderer = GameTechRenderer::instance();
	MenuManager& menuManager = MenuManager::instance();
	InputKeyMap& keyMap = NCL::InputKeyMap::instance();

	std::unique_ptr<TutorialGame> game;

	ChangeState menuState = ChangeState::None;
};
