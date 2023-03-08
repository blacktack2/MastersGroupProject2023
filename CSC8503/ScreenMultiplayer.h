/**
 * @file   ScreenMultiplayer.h
 * @brief  A Pushdown automata state for running a multiplayer game.
 *
 * @author Felix Chiu
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "PushdownState.h"
#include "Window.h"
#include "MenuManager.h"
#include "ScreenMultiplayer.h"
#include "NetworkedGame.h"
#include "ScreenPause.h"

#include <memory>

using namespace NCL;
using namespace CSC8503;

class ScreenMultiplayer : public PushdownState {
public:
	ScreenMultiplayer(bool isServer = true);
	~ScreenMultiplayer() = default;

	PushdownResult OnUpdate(float dt, PushdownState** newState) override;
	void OnAwake() override;

private:
	enum class ChangeState {
		None,
		OnGoing,
		Start,
		Quit
	};
	
	GameStateManager& gameStateManager = GameStateManager::instance();
	GameTechRenderer& renderer = GameTechRenderer::instance();
	MenuManager& menuManager = MenuManager::instance();
	paintHell::InputKeyMap& keyMap = paintHell::InputKeyMap::instance();

	std::unique_ptr<NetworkedGame> game;

	ChangeState menuState = ChangeState::None;
};
