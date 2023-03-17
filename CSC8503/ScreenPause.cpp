/**
 * @file   ScreenPause.cpp
 * @brief  See ScreenPause.h .
 *
 * @author Felix Chiu
 * @author Yifei Hu
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
	keyMap.Update();
	menuManager.Update(dt);

	renderer.Render();
	if (game) {
		NetworkedGame* netGame = dynamic_cast<NetworkedGame*>(game);
		if (netGame) {
			netGame->FreezeSelf();
		}
		game->UpdateGame(dt);
	}

	switch (menuState)
	{
	case ChangeState::Resume:
		if (game) {
			NetworkedGame* netGame = dynamic_cast<NetworkedGame*>(game);
			if (netGame) {
				netGame->UnfreezeSelf();
			}
		}
		return PushdownResult::Pop;
	case ChangeState::Option:
		*newState = new ScreenOption();
		return PushdownResult::Push;
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
	renderer.EnableOverlayPass("Hud", false);
	renderer.UpdatePipeline();

	Window::GetWindow()->ShowOSPointer(true);
	Window::GetWindow()->LockMouseToWindow(false);
}

void ScreenPause::InitMenu() {
	Menu& menu = menuManager.AddMenu(NAME, Vector2(0.0f), Vector2(0.3f, 0.7f), AssetLibrary<TextureBase>::GetAsset("menuPause"));

	menu.AddButton(0.0f, 0.45f, 0.16f, 0.08f, AssetLibrary<TextureBase>::GetAsset("button4"), [&](Button& button) {
		std::cout << "Resume button clicked\n";
		menuState = ChangeState::Resume;
		}, [&](Button& button) {
			ResumePointer = false;
			if (optionManager.GetCounter() % 4 == 1) {
				std::cout << "Resume button selected\n";
				ResumePointer = true;
			}
			ResumePointer ? button.SetTexture(AssetLibrary<TextureBase>::GetAsset("buttonSlide4")) : button.SetTexture(AssetLibrary<TextureBase>::GetAsset("button4"));
		}, [&](Button& button) {
			if (ResumePointer) {
				std::cout << "Resume button pressed\n";
				menuState = ChangeState::Resume;
			}
			ResumePointer = false;
			button.SetTexture(AssetLibrary<TextureBase>::GetAsset("button4"));
	});

	menu.AddButton(0.0f, 0.15f, 0.16f, 0.08f, AssetLibrary<TextureBase>::GetAsset("button5"), [&](Button& button) {
		std::cout << "Option button clicked\n";
		menuState = ChangeState::Option;
		}, [&](Button& button) {
			OptionPointer = false;
			if (optionManager.GetCounter() % 4 == 2) {
				std::cout << "Option button selected\n";
				OptionPointer = true;
			}
			OptionPointer ? button.SetTexture(AssetLibrary<TextureBase>::GetAsset("buttonSlide5")) : button.SetTexture(AssetLibrary<TextureBase>::GetAsset("button5"));
		}, [&](Button& button) {
			if (OptionPointer) {
				std::cout << "Option button pressed\n";
				menuState = ChangeState::Option;
			}
			OptionPointer = false;
			button.SetTexture(AssetLibrary<TextureBase>::GetAsset("button5"));
		});

	menu.AddButton(0.0f, -0.15f, 0.16f, 0.08f, AssetLibrary<TextureBase>::GetAsset("button6"), [&](Button& button) {
		std::cout << "Dummy button clicked\n";
		// TODO - Add functionality
		}, [&](Button& button) {
			TitlePointer = false;
			if (optionManager.GetCounter() % 4 == 3) {
				std::cout << "Title button selected\n";
				TitlePointer = true;
			}
			TitlePointer ? button.SetTexture(AssetLibrary<TextureBase>::GetAsset("buttonSlide6")) : button.SetTexture(AssetLibrary<TextureBase>::GetAsset("button6"));
		}, [&](Button& button) {
			if (TitlePointer) {
				std::cout << "Title button pressed\n";
				// TODO - Add functionality
			}
			TitlePointer = false;
			button.SetTexture(AssetLibrary<TextureBase>::GetAsset("button6"));
		});

	menu.AddButton(0.0f, -0.45f, 0.16f, 0.08f, AssetLibrary<TextureBase>::GetAsset("button7"), [&](Button& button) {
		std::cout << "Quit button clicked\n";
		gameStateManager.SetGameState(GameState::Quit);
		menuState = ChangeState::Quit;
		}, [&](Button& button) {
			QuitPointer = false;
			if (optionManager.GetCounter() % 4 == 0) {
				std::cout << "Quit button selected\n";
				QuitPointer = true;
			}
			QuitPointer ? button.SetTexture(AssetLibrary<TextureBase>::GetAsset("buttonSlide7")) : button.SetTexture(AssetLibrary<TextureBase>::GetAsset("button7"));
		}, [&](Button& button) {
			if (QuitPointer) {
				std::cout << "Quit button pressed\n";
				gameStateManager.SetGameState(GameState::Quit);
				menuState = ChangeState::Quit;
			}
			QuitPointer = false;
			button.SetTexture(AssetLibrary<TextureBase>::GetAsset("button7"));
		});
}