/**
 * @file   ScreenMain.h
 * @brief  A class for defining the main menu.
 *
 * @author Felix Chiu
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "PushdownState.h"
#include "Window.h"
#include "MenuManager.h"
#include "ScreenGame.h"
#include "ScreenOption.h"
#include "ScreenMultiplayerOption.h"

using namespace NCL;
using namespace NCL::CSC8503;

class ScreenMain : public PushdownState {
public:
	ScreenMain();
	~ScreenMain() = default;

	PushdownResult OnUpdate(float dt, PushdownState** newState) override;
	void OnAwake() override;
private:
	enum class ChangeState {
		None,
		OnGoing,
		Start,
		Multiplayer,
		Option,
		Quit,
	};

	void InitMenu();

	GameTechRenderer& renderer = GameTechRenderer::instance();
	MenuManager& menuManager = MenuManager::instance();
	NCL::InputKeyMap& keyMap = NCL::InputKeyMap::instance();

	ChangeState menuState = ChangeState::None;

	const std::string NAME = "main";
};
