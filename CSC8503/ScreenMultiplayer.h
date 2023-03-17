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

#ifdef _ORBIS
#include "../CSC8503/PS4Renderer.h"
#endif // _ORBIS


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
#ifdef x64
	GameTechRenderer& renderer = GameTechRenderer::instance();
#endif // x64
#ifdef _ORBIS
	NCL::PS4::PS4Renderer* renderer = new NCL::PS4::PS4Renderer();
#endif // _ORBIS

	
	MenuManager& menuManager = MenuManager::instance();
	NCL::InputKeyMap& keyMap = NCL::InputKeyMap::instance();

	std::unique_ptr<NetworkedGame> game;

	ChangeState menuState = ChangeState::None;
};
