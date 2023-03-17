/**
 * @file   ScreenPause.h
 * @brief  A Pushdown automata state for Pausing the game.
 *
 * @author Felix Chiu
 * @author Yifei Hu
 * @date   February 2023
 */
#pragma once
#include "PushdownState.h"
#include "Window.h"
#include "MenuManager.h"
#include "ScreenOption.h"
#include "GameStateManager.h"
#include "NetworkedGame.h"

#ifdef _ORBIS
#include "../CSC8503/PS4Renderer.h"
#endif //_ORBIS

using namespace NCL;
using namespace CSC8503;

class ScreenPause : public PushdownState {
public:
	ScreenPause(TutorialGame* game = nullptr);
	~ScreenPause();
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

	TutorialGame* game;

	GameStateManager& gameStateManager = GameStateManager::instance();
#ifdef x64
	GameTechRenderer& renderer = GameTechRenderer::instance();
#endif //x64
#ifdef _ORBIS
	PS4Renderer* renderer = new PS4Renderer();
#endif //ORBIS
	MenuManager& menuManager = MenuManager::instance();
	NCL::InputKeyMap& keyMap = NCL::InputKeyMap::instance();
	OptionManager& optionManager = OptionManager::instance();

	ChangeState menuState = ChangeState::None;

	std::string NAME = "pause";

	bool ResumePointer = false;
	bool OptionPointer = false;
	bool TitlePointer = false;
	bool QuitPointer = false;
};
