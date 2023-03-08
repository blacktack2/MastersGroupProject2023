/**
 * @file   ScreenPause.cpp
 * @brief  See ScreenPause.h .
 *
 * @author Felix Chiu
 * @date   February 2023
 */
#pragma once
#include "ScreenPause.h"

#include "AssetLibrary.h"
#include "MenuRenderObject.h"

#include <functional>
#include <optional>

using namespace NCL;
using namespace CSC8503;

ScreenPause::ScreenPause(std::optional<std::reference_wrapper<TutorialGame>> game) {
	this->game = game;
	InitMenu();
}

PushdownState::PushdownResult ScreenPause::OnUpdate(float dt, PushdownState** newState) {
	menuManager.Update(dt);
	keyMap.Update();
	renderer.Render();
	if (game) {
		NetworkedGame* netGame = dynamic_cast<NetworkedGame*>(&game.value().get());
		if (netGame) {
			netGame->FreezeSelf();
		}
		game.value().get().UpdateGame(dt);
	}
	switch (menuState) {
		case ChangeState::Resume:
		case ChangeState::Quit:
			if (game) {
				NetworkedGame* netGame = dynamic_cast<NetworkedGame*>(&game.value().get());
				if (netGame) {
					netGame->FreezeSelf();
				}
			}
			return PushdownResult::Pop;
		default:
			return PushdownResult::NoChange;
	}
}

void ScreenPause::OnAwake() {
	menuState = ChangeState::None;

	menuManager.SetCurrentMenu(NAME);

	renderer.EnableOverlayPass("Menu", true);
	renderer.EnableOverlayPass("Debug", false);
	renderer.UpdatePipeline();

	Window::GetWindow()->ShowOSPointer(true);
	Window::GetWindow()->LockMouseToWindow(false);
}

void ScreenPause::InitMenu() {
	Menu& menu = menuManager.AddMenu(NAME, Vector2(0.0f), Vector2(0.3f, 0.7f), AssetLibrary::GetTexture("menuPause"));

	menu.AddButton(0.0f, 0.45f, 0.16f, 0.08f, AssetLibrary::GetTexture("button4"), [&]() {
		std::cout << "Resume button clicked\n";
		menuState = ChangeState::Resume;
	});
	menu.AddButton(0.0f, 0.15f, 0.16f, 0.08f, AssetLibrary::GetTexture("button5"), [&]() {
		std::cout << "Dummy button clicked\n";
		// TODO - Add functionality
	});
	menu.AddButton(0.0f, -0.15f, 0.16f, 0.08f, AssetLibrary::GetTexture("button6"), [&]() {
		std::cout << "Dummy button clicked\n";
		// TODO - Add functionality
	});
	menu.AddButton(0.0f, -0.45f, 0.16f, 0.08f, AssetLibrary::GetTexture("button7"), [&]() {
		std::cout << "Quit button clicked\n";
		gameStateManager.SetGameState(GameState::Quit);
		menuState = ChangeState::Quit;
	});
}