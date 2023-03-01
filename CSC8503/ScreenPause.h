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
	ScreenPause(TutorialGame* game = nullptr) {
		this->game = game;
		initMenu();
	}
	~ScreenPause() {}
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		menuManager.Update(dt);
		keyMap.Update();
		renderer.Render();
		if (game) {
			if (static_cast<NetworkedGame*> (game)) {
				static_cast<NetworkedGame*> (game)->FreezeSelf();
			}
			game->UpdateGame(dt);
		}
		switch (menuState)
		{
		case ChangeState::Resume:
		case ChangeState::Quit:
			if (game) {
				if (static_cast<NetworkedGame*> (game)) {
					static_cast<NetworkedGame*> (game)->UnfreezeSelf();
				}
			}
			return PushdownResult::Pop;
		default:
			return PushdownResult::NoChange;
		}
	}
	void OnAwake() override {
		menuState = ChangeState::None;
		menuManager.SetCurrentMenu(name);
		renderer.EnableOverlayPass("Menu", true);
		renderer.EnableOverlayPass("Debug", false);
		renderer.UpdatePipeline();
		Window::GetWindow()->ShowOSPointer(true);
		Window::GetWindow()->LockMouseToWindow(false);
	}

private:
	void initMenu();

	enum class ChangeState {
		None,
		OnGoing,
		Resume,
		Option,
		Quit
	};
	TutorialGame* game;

	GameStateManager* gameStateManager = &GameStateManager::instance();
	GameTechRenderer& renderer = GameTechRenderer::instance();
	MenuManager& menuManager = MenuManager::instance();
	paintHell::InputKeyMap& keyMap = paintHell::InputKeyMap::instance();

	ChangeState menuState = ChangeState::None;

	string name = "pause";
};
