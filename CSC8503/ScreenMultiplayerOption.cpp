/**
 * @file   ScreenMultiplayerOption.cpp
 * @brief  See ScreenMultiplayerOption.h.
 *
 * @author Felix Chiu
 * @author Yifei Hu
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "ScreenMultiplayerOption.h"

#include "AssetLibrary.h"

using namespace NCL;
using namespace NCL::CSC8503;

ScreenMultiplayerOption::ScreenMultiplayerOption() {
	InitMenu();
}

ScreenMultiplayerOption::~ScreenMultiplayerOption() {
	menuManager.RemoveAndEraseMenu(NAME);
}

PushdownState::PushdownResult ScreenMultiplayerOption::OnUpdate(float dt, PushdownState** newState) {
	menuManager.Update(dt);
	keyMap.Update();
	renderer.Render();
	switch (menuState) {
	case ChangeState::StartServer:
		*newState = new ScreenMultiplayer(true);
		return PushdownResult::Push;
	case ChangeState::StartClient:
		*newState = new ScreenMultiplayer(false);
		return PushdownResult::Push;
	case ChangeState::Quit:
		return PushdownResult::Pop;
	default:
		menuState = ChangeState::OnGoing;
		return PushdownResult::NoChange;
	}
}

void ScreenMultiplayerOption::OnAwake() {
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

void ScreenMultiplayerOption::InitMenu() {
	Menu& menu = menuManager.AddMenu(NAME, Vector2(0.0f), Vector2(1.0f), AssetLibrary<TextureBase>::GetAsset("menuMain"));

	menu.AddButton(0.5f, 0.0f, 0.2f, 0.1f, AssetLibrary<TextureBase>::GetAsset("button0"), [&](Button& button) {
		std::cout << "Start server button clicked\n";
		menuState = ChangeState::StartServer;
		}, [&](Button& button) {
			StartPointer = false;
			if (optionManager.GetCounter() % 3 == 1) {
				std::cout << "Start button selected\n";
				StartPointer = true;
			}
			StartPointer ? button.SetTexture(AssetLibrary<TextureBase>::GetAsset("buttonSlide0")) : button.SetTexture(AssetLibrary<TextureBase>::GetAsset("button0"));
		}, [&](Button& button) {
			if (StartPointer) {
				std::cout << "Start button pressed\n";
				menuState = ChangeState::StartServer;
			}
			StartPointer = false;
			button.SetTexture(AssetLibrary<TextureBase>::GetAsset("button0"));
		});

	menu.AddButton(0.5f, -0.3f, 0.2f, 0.1f, AssetLibrary<TextureBase>::GetAsset("button1"), [&](Button& button) {
		std::cout << "Start client button clicked\n";
		menuState = ChangeState::StartClient;
		}, [&](Button& button) {
			MultiPointer = false;
			if (optionManager.GetCounter() % 3 == 2) {
				std::cout << "Start button selected\n";
				MultiPointer = true;
			}
			MultiPointer ? button.SetTexture(AssetLibrary<TextureBase>::GetAsset("buttonSlide1")) : button.SetTexture(AssetLibrary<TextureBase>::GetAsset("button1"));
		}, [&](Button& button) {
			if (MultiPointer) {
				std::cout << "Start button pressed\n";
				menuState = ChangeState::StartClient;
			}
			MultiPointer = false;
			button.SetTexture(AssetLibrary<TextureBase>::GetAsset("button1"));
		});

	menu.AddButton(0.5f, -0.6f, 0.2f, 0.1f, AssetLibrary<TextureBase>::GetAsset("button3"), [&](Button& button) {
		std::cout << "Quit button clicked\n";
		menuState = ChangeState::Quit;
		}, [&](Button& button) {
			QuitPointer = false;
			if (optionManager.GetCounter() % 3 == 0) {
				std::cout << "Quit button selected\n";
				QuitPointer = true;
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