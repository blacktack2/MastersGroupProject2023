/**
 * @file   ScreenGame.cpp
 * @brief  See ScreenGame.h .
 *
 * @author Felix Chiu
 * @date   February 2023
 */
#pragma once
#include "ScreenGame.h"

using namespace NCL;
using namespace NCL::CSC8503;

ScreenGame::ScreenGame() {
	game = std::make_unique<TutorialGame>();
	gameStateManager.SetGameState(GameState::OnGoing);
}

PushdownState::PushdownResult ScreenGame::OnUpdate(float dt, PushdownState** newState) {
	keyMap.Update();
	if (keyMap.GetButton(InputType::ESC)) {
		*newState = new ScreenPause();
		return PushdownResult::Push;
	}
	if (game->IsQuit()) {
		menuState = ChangeState::Quit;
	} else {
		game->UpdateGame(dt);
	}
	if (menuState == ChangeState::Quit) {
		/*
		Window::GetWindow()->ShowOSPointer(true);
		Window::GetWindow()->LockMouseToWindow(false);
		renderer.EnableOverlayPass("Menu", true);
		renderer.UpdatePipeline();
		*/
		return PushdownResult::Pop;
	}


	return PushdownResult::NoChange;
}

void ScreenGame::OnAwake() {
	menuState = ChangeState::None;

	renderer.EnableRenderScene(true);
	renderer.EnableOverlayPass("Menu", false);
	renderer.EnableOverlayPass("Debug", true);
	renderer.EnableOverlayPass("Hud", true);
	renderer.UpdatePipeline();

	Window::GetWindow()->ShowOSPointer(false);
	Window::GetWindow()->LockMouseToWindow(true);
}
