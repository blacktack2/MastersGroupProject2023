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

ScreenPause::ScreenPause(TutorialGame* game) {
	this->game = game;
	InitMenu();
}

ScreenPause::~ScreenPause() {
	menuManager.RemoveAndEraseMenu(NAME);
}

PushdownState::PushdownResult ScreenPause::OnUpdate(float dt, PushdownState** newState) {
	menuManager.Update(dt);
	keyMap.Update();
	renderer.Render();
	if (game) {
		NetworkedGame* netGame = dynamic_cast<NetworkedGame*>(game);
		if (netGame) {
			netGame->FreezeSelf();
		}
		game->UpdateGame(dt);
	}
	switch (menuState) {
		case ChangeState::Resume:
		case ChangeState::Quit:
			if (game) {
				NetworkedGame* netGame = dynamic_cast<NetworkedGame*>(game);
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
	Menu& menu = menuManager.AddMenu(NAME, Vector2(0.0f), Vector2(0.3f, 0.7f), AssetLibrary<TextureBase>::GetAsset("menuPause"));

	menu.AddButton(0.0f, 0.45f, 0.16f, 0.08f, AssetLibrary<TextureBase>::GetAsset("button4"), [&]() {
		std::cout << "Resume button clicked\n";
		menuState = ChangeState::Resume;
	});
	menu.AddButton(0.0f, 0.15f, 0.16f, 0.08f, AssetLibrary<TextureBase>::GetAsset("button5"), [&]() {
		std::cout << "Dummy button clicked\n";
		// TODO - Add functionality
	});
	menu.AddButton(0.0f, -0.15f, 0.16f, 0.08f, AssetLibrary<TextureBase>::GetAsset("button6"), [&]() {
		std::cout << "Dummy button clicked\n";
		// TODO - Add functionality
	});
	menu.AddButton(0.0f, -0.45f, 0.16f, 0.08f, AssetLibrary<TextureBase>::GetAsset("button7"), [&]() {
		std::cout << "Quit button clicked\n";
		gameStateManager.SetGameState(GameState::Quit);
		menuState = ChangeState::Quit;
	});
}