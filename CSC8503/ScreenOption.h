/**
 * @file   ScreenOption.h
 * @brief  A Pushdown automata state for game options.
 *
 * @author Felix Chiu
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "PushdownState.h"
#include "Window.h"
#include "MenuManager.h"

using namespace NCL;
using namespace CSC8503;

class ScreenOption : public PushdownState {
public:
	ScreenOption();
	~ScreenOption() = default;
	PushdownResult OnUpdate(float dt, PushdownState** newState) override;
	void OnAwake() override;

	GameTechRenderer& renderer = GameTechRenderer::instance();
	MenuManager& menuManager = MenuManager::instance();
	NCL::InputKeyMap& keyMap = NCL::InputKeyMap::instance();

private:
	enum class ChangeState {
		None,
		OnGoing,
		Resume,
		Option,
		Quit
	};

	void InitMenu();

	ChangeState menuState = ChangeState::None;

	const std::string NAME = "option";
};
