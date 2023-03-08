/**
 * @file   ScreenMultiplayerOption.h
 * @brief  A Pushdown automata state for multiplayer game options.
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

using namespace NCL;
using namespace CSC8503;

class ScreenMultiplayerOption : public PushdownState {
public:
	ScreenMultiplayerOption();
	~ScreenMultiplayerOption() = default;
	PushdownResult OnUpdate(float dt, PushdownState** newState) override;
	void OnAwake() override;

	GameTechRenderer& renderer = GameTechRenderer::instance();
	MenuManager& menuManager = MenuManager::instance();
	NCL::InputKeyMap& keyMap = NCL::InputKeyMap::instance();

private:
	void InitMenu();

	enum class ChangeState {
		None,
		OnGoing,
		StartServer,
		StartClient,
		Quit
	};
	ChangeState menuState = ChangeState::None;

	const std::string NAME = "ScreenMultiplayerOption";
};
