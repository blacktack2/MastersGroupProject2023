/**
 * @file   ScreenOption.h
 * @brief  A Pushdown automata state for game options.
 *
 * @author Felix Chiu
 * @author Yifei Hu
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "PushdownState.h"
#include "Window.h"
#include "MenuManager.h"
#include "OptionManager.h"

#include "DebugViewPoint.h"

#include <functional>

#ifdef _ORBIS
#include "../CSC8503/PS4Renderer.h"
#endif // _ORBIS


using namespace NCL;
using namespace CSC8503;

class ScreenOption : public PushdownState {
public:
	ScreenOption();
	~ScreenOption();
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
#ifdef x64
	GameTechRenderer& renderer = GameTechRenderer::instance();
#endif
#ifdef _ORBIS
	PS4Renderer* renderer = new PS4Renderer();
#endif
	MenuManager& menuManager = MenuManager::instance();
	InputKeyMap& keyMap = InputKeyMap::instance();
	OptionManager& optionManager = OptionManager::instance();

	ChangeState menuState = ChangeState::None;

	const std::string NAME = "option";

	int up = 0;
	int down = 0;

	bool ReturnPointer = false;
	bool SunMovePointer = false;
	bool DebugModePointer = false;
	bool SoundPointer = false;
	bool VolumeUpPointer = false;
	bool VolumeDownPointer = false;
};
