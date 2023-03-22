/**
 * @file   ScreenAddress.h
 * @brief  A Pushdown automata state for changing networked IP address.
 *
 * @author Felix Chiu
 * @date   March 2023
 */
#pragma once
#include "PushdownState.h"
#include "Window.h"
#include "MenuManager.h"
#include "OptionManager.h"

#include "DebugViewPoint.h"

#include <functional>

using namespace NCL;
using namespace CSC8503;

#define NUMBTN 13

class ScreenAddress : public PushdownState {
public:
	ScreenAddress();
	~ScreenAddress();
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

	GameTechRenderer& renderer = GameTechRenderer::instance();
	MenuManager& menuManager = MenuManager::instance();
	InputKeyMap& keyMap = InputKeyMap::instance();
	OptionManager& optionManager = OptionManager::instance();

	ChangeState menuState = ChangeState::None;

	const std::string NAME = "address";

	int up = 0;
	int down = 0;

	bool num[NUMBTN]= {false};
	bool confirm = false;
};
