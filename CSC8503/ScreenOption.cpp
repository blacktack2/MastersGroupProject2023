/**
 * @file   ScreenOption.cpp
 * @brief  See ScreenOption.h.
 *
 * @author Felix Chiu
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "ScreenOption.h"

#include "AssetLibrary.h"
#include "MenuRenderObject.h"

using namespace NCL;
using namespace CSC8503;

ScreenOption::ScreenOption() {
	InitMenu();
	OnAwake();
}

ScreenOption::~ScreenOption() {
	menuManager.RemoveAndEraseMenu(NAME);
}

PushdownState::PushdownResult ScreenOption::OnUpdate(float dt, PushdownState** newState) {
	menuManager.Update(dt);
	keyMap.Update();
	renderer.Render();
	if (menuState == ChangeState::Resume || menuState == ChangeState::Quit) {
		return PushdownResult::Pop;
	}
	menuState = ChangeState::OnGoing;
	return PushdownResult::NoChange;
}

void ScreenOption::OnAwake() {
	menuState = ChangeState::None;

	menuManager.SetCurrentMenu(NAME);

	renderer.EnableOverlayPass("Menu", true);
	renderer.EnableRenderScene(false);
	renderer.UpdatePipeline();

	Window::GetWindow()->ShowOSPointer(true);
	Window::GetWindow()->LockMouseToWindow(false);
}

void ScreenOption::InitMenu() {
	Menu& menu = menuManager.AddMenu(NAME, Vector2(0.0f), Vector2(1.0f), AssetLibrary<TextureBase>::GetAsset("menuOption"));

	menu.AddButton(0.0f, 0.45f, 0.16f, 0.08f, AssetLibrary<TextureBase>::GetAsset("button4"), [&]() {
		std::cout << "Resume button clicked\n";
		menuState = ChangeState::Resume;
	});
	menu.AddButton(0.0f, 0.15f, 0.16f, 0.08f, AssetLibrary<TextureBase>::GetAsset("button5"), [&]() {
		std::cout << "Dummy button clicked\n";
		// TODO - Add functionality
	});
	menu.AddButton(0.0f, -0.15f, 0.16f, 0.08f, AssetLibrary<TextureBase>::GetAsset("button5"), [&]() {
		std::cout << "Dummy button clicked\n";
		// TODO - Add functionality
	});
	menu.AddButton(0.0f, -0.45f, 0.16f, 0.08f, AssetLibrary<TextureBase>::GetAsset("button6"), [&]() {
		std::cout << "Quit button clicked\n";
		menuState = ChangeState::Quit;
	});
}