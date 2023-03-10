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
	Menu& menu = menuManager.AddMenu(NAME, Vector2(0.0f), Vector2(1.0f), AssetLibrary::GetTexture("menuMain"));

	menu.AddButton(0.5f, 0.3f, 0.2f, 0.1f, AssetLibrary::GetTexture("button0"), [&](Button& button) {
		std::cout << "Start button clicked\n";
		menuState = ChangeState::Start;
		});
	menu.AddButton(0.5f, 0.0f, 0.2f, 0.1f, AssetLibrary::GetTexture("button1"), [&](Button& button) {
		std::cout << "Multiplayer button clicked\n";
		menuState = ChangeState::Multiplayer;
		});
	menu.AddButton(0.5f, -0.3f, 0.2f, 0.1f, AssetLibrary::GetTexture("button2"), [&](Button& button) {
		std::cout << "Option button clicked\n";
		menuState = ChangeState::Option;
		});
	menu.AddButton(0.5f, -0.6f, 0.2f, 0.1f, AssetLibrary::GetTexture("button3"), [&](Button& button) {
		std::cout << "Quit button clicked\n";
		menuState = ChangeState::Quit;
		});
}
