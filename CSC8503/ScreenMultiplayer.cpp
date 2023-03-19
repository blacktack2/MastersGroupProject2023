/**
 * @file   ScreenMultiplayer.cpp
 * @brief  See ScreenMultiplayer.h .
 *
 * @author Felix Chiu
 * @date   February 2023
 */
#pragma once
#include "ScreenMultiplayer.h"

ScreenMultiplayer::ScreenMultiplayer(bool isServer) {
	game = std::make_unique<NetworkedGame>(isServer);
}

PushdownState::PushdownResult ScreenMultiplayer::OnUpdate(float dt, PushdownState** newState) {
	keyMap.Update();
	if (keyMap.GetButton(InputType::Pause)) {
		*newState = new ScreenPause(game.get());
		return PushdownResult::Push;
	}

	if (game->IsQuit()) {
		menuState = ChangeState::Quit;
	} else {
		game->UpdateGame(dt);
	}

	if (menuState == ChangeState::Quit) {
		Window::GetWindow()->ShowOSPointer(true);
		Window::GetWindow()->LockMouseToWindow(false);
		renderer.EnableOverlayPass("Menu", true);
		renderer.UpdatePipeline();
		return PushdownResult::Pop;
	}
	return PushdownResult::NoChange;
}

void ScreenMultiplayer::OnAwake() {
	menuState = ChangeState::None;

	renderer.EnableRenderScene(true);
	renderer.EnableOverlayPass("Menu", false);
	renderer.EnableOverlayPass("Debug", true);
	renderer.EnableOverlayPass("Hud", true);
	renderer.UpdatePipeline();

	Window::GetWindow()->ShowOSPointer(false);
	Window::GetWindow()->LockMouseToWindow(true);
}
