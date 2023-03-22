/**
 * @file   ScreenMain.h
 * @brief  A class for defining the main menu.
 *
 * @author Felix Chiu
 * @author Yifei Hu
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "ScreenMain.h"

#include "AssetLibrary.h"
#include "MenuRenderObject.h"

using namespace NCL;
using namespace NCL::CSC8503;

ScreenMain::ScreenMain() {
	InitMenu();
	OnAwake();
}
ScreenMain::~ScreenMain() {
	menuManager.RemoveAndEraseMenu(NAME);
}

PushdownState::PushdownResult ScreenMain::OnUpdate(float dt, PushdownState** newState) {
	menuManager.Update(dt);
	keyMap.Update();
	renderer.Render();
	switch (menuState) {
	case ChangeState::Start:
		*newState = new ScreenGame();
		return PushdownResult::Push;
	case ChangeState::Multiplayer:
		*newState = new ScreenMultiplayerOption();
		return PushdownResult::Push;
	case ChangeState::Option:
		*newState = new ScreenOption();
		return PushdownResult::Push;
	case ChangeState::Quit:
		return PushdownResult::Pop;
	default:
		menuState = ChangeState::OnGoing;
		return PushdownResult::NoChange;
	}
}

void ScreenMain::OnAwake() {
	menuState = ChangeState::None;

	menuManager.SetCurrentMenu(NAME);

	renderer.EnableOverlayPass("Menu", true);
	renderer.EnableOverlayPass("Debug", false);
	renderer.EnableOverlayPass("Hud", false);
	renderer.EnableRenderScene(false);
	renderer.UpdatePipeline();

	Window::GetWindow()->ShowOSPointer(true);
	Window::GetWindow()->LockMouseToWindow(false);
}

void ScreenMain::InitMenu() {
	Menu& menu = menuManager.AddMenu(NAME, Vector2(0.0f), Vector2(1.0f), AssetLibrary<TextureBase>::GetAsset("menuMain"));

	menu.AddButton(0.5f, 0.3f, 0.2f, 0.1f, AssetLibrary<TextureBase>::GetAsset("button0"), [&](Button& button) {
		std::cout << "Start button clicked\n";
		menuState = ChangeState::Start;
		}, [&](Button& button) {
			StartPointer = false; 
			if (optionManager.GetCounter() > 0) {
				if (optionManager.GetCounter() % 4 == 1) {
					std::cout << "Start button selected\n";
					StartPointer = true;
				}
			}
			else if (optionManager.GetCounter() <= 0) {
				if (optionManager.GetCounter() % 4 == 0) {
					std::cout << "Start button selected\n";
					StartPointer = true;
				}
			}
			StartPointer ? button.SetTexture(AssetLibrary<TextureBase>::GetAsset("buttonSlide0")) : button.SetTexture(AssetLibrary<TextureBase>::GetAsset("button0"));
		}, [&](Button& button) {
			if (StartPointer) {
				std::cout << "Start button pressed\n";
				menuState = ChangeState::Start;
			}
			StartPointer = false;
			button.SetTexture(AssetLibrary<TextureBase>::GetAsset("button0"));
	});

	menu.AddButton(0.5f, 0.0f, 0.2f, 0.1f, AssetLibrary<TextureBase>::GetAsset("button1"), [&](Button& button) {
		std::cout << "Multiplayer button clicked\n";
		menuState = ChangeState::Multiplayer;
		}, [&](Button& button) {
			MultiPointer = false;
			if (optionManager.GetCounter() > 0) {
				if (optionManager.GetCounter() % 4 == 2) {
					std::cout << "Multiplayer button selected\n";
					MultiPointer = true;
				}
			}
			else if (optionManager.GetCounter() <= 0) {
				if (optionManager.GetCounter() % 4 == -3) {
					std::cout << "Multiplayer button selected\n";
					MultiPointer = true;
				}
			}
			MultiPointer ? button.SetTexture(AssetLibrary<TextureBase>::GetAsset("buttonSlide1")) : button.SetTexture(AssetLibrary<TextureBase>::GetAsset("button1"));
		}, [&](Button& button) {
			if (MultiPointer) {
				std::cout << "Multiplayer button pressed\n";
				menuState = ChangeState::Multiplayer;
			}
			MultiPointer = false;
			button.SetTexture(AssetLibrary<TextureBase>::GetAsset("button1"));
	});

	menu.AddButton(0.5f, -0.3f, 0.2f, 0.1f, AssetLibrary<TextureBase>::GetAsset("button2"), [&](Button& button) {
		std::cout << "Option button clicked\n";
		menuState = ChangeState::Option;
		}, [&](Button& button) {
			OptionPointer = false;
			if (optionManager.GetCounter() > 0) {
				if (optionManager.GetCounter() % 4 == 3) {
					std::cout << "Option button selected\n";
					OptionPointer = true;
				}
			}
			else if (optionManager.GetCounter() <= 0) {
				if (optionManager.GetCounter() % 4 == -2) {
					std::cout << "Option button selected\n";
					OptionPointer = true;
				}
			}
			OptionPointer ? button.SetTexture(AssetLibrary<TextureBase>::GetAsset("buttonSlide2")) : button.SetTexture(AssetLibrary<TextureBase>::GetAsset("button2"));
		}, [&](Button& button) {
			if (OptionPointer) {
				std::cout << "Option button pressed\n";
				menuState = ChangeState::Option;
			}
			OptionPointer = false;
			button.SetTexture(AssetLibrary<TextureBase>::GetAsset("button2"));
	});

	menu.AddButton(0.5f, -0.6f, 0.2f, 0.1f, AssetLibrary<TextureBase>::GetAsset("button3"), [&](Button& button) {
		std::cout << "Quit button clicked\n";
		menuState = ChangeState::Quit;
		}, [&](Button& button) {
			QuitPointer = false;
			if (optionManager.GetCounter() > 0) {
				if (optionManager.GetCounter() % 4 == 0) {
					std::cout << "Quit button selected\n";
					QuitPointer = true;
				}
			}
			else if (optionManager.GetCounter() <= 0) {
				if (optionManager.GetCounter() % 4 == -1) {
					std::cout << "Quit button selected\n";
					QuitPointer = true;
				}
			}
			QuitPointer ? button.SetTexture(AssetLibrary<TextureBase>::GetAsset("buttonSlide3")) : button.SetTexture(AssetLibrary<TextureBase>::GetAsset("button3"));
		}, [&](Button& button) {
			if (QuitPointer) {
				std::cout << "Quit button pressed\n";
				menuState = ChangeState::Quit;
			}
			QuitPointer = false;
			button.SetTexture(AssetLibrary<TextureBase>::GetAsset("button3"));
	});
}