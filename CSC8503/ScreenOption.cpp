/**
 * @file   ScreenOption.cpp
 * @brief  See ScreenOption.h.
 *
 * @author Felix Chiu
 * @author Yifei Hu
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "ScreenOption.h"
#include "AssetLibrary.h"

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
	renderer.UpdatePipeline();

	Window::GetWindow()->ShowOSPointer(true);
	Window::GetWindow()->LockMouseToWindow(false);
}

void ScreenOption::InitMenu() {
	Menu& menu = menuManager.AddMenu(NAME, Vector2(0.0f), Vector2(1.0f), AssetLibrary<TextureBase>::GetAsset("menuOption"));

	menu.AddButton(0.88f, -0.8f, 0.06f, 0.12f, AssetLibrary<TextureBase>::GetAsset("return"), [&](Button& button) {
		std::cout << "Resume button clicked\n";
		menuState = ChangeState::Resume;
	});

	std::shared_ptr<TextureBase> SunMoveTex = !optionManager.GetSunMove() ?
		AssetLibrary<TextureBase>::GetAsset("checkbox0") : AssetLibrary<TextureBase>::GetAsset("checkbox1");
	menu.AddButton(0.693f, 0.45f, 0.02f, 0.04f, SunMoveTex, [&](Button& button) {
		std::cout << "sun move button clicked\n";
		optionManager.SetSunMove(!optionManager.GetSunMove());
		button.SetTexture(
			!optionManager.GetSunMove() ? AssetLibrary<TextureBase>::GetAsset("checkbox0") : AssetLibrary<TextureBase>::GetAsset("checkbox1")
		);
	});

	std::shared_ptr<TextureBase> DebugModeTex = !optionManager.GetDebugMode() ?
		AssetLibrary<TextureBase>::GetAsset("checkbox0") : AssetLibrary<TextureBase>::GetAsset("checkbox1");
	menu.AddButton(0.693f, -0.085f, 0.02f, 0.04f, DebugModeTex, [&](Button& button) {
		std::cout << "debug mode button clicked\n";
		optionManager.SetDebugMode(!optionManager.GetDebugMode());
		button.SetTexture(
			!optionManager.GetDebugMode() ? AssetLibrary<TextureBase>::GetAsset("checkbox0") : AssetLibrary<TextureBase>::GetAsset("checkbox1")
		);
	});

	std::shared_ptr<TextureBase> SoundTex = !optionManager.GetSound() ?
		AssetLibrary<TextureBase>::GetAsset("checkbox0") : AssetLibrary<TextureBase>::GetAsset("checkbox1");
	menu.AddButton(0.693f, -0.595f, 0.02f, 0.04f, SoundTex, [&](Button& button) {
		std::cout << "sound button clicked\n";
		optionManager.SetSound(!optionManager.GetSound());
		button.SetTexture(
			!optionManager.GetSound() ? AssetLibrary<TextureBase>::GetAsset("checkbox0") : AssetLibrary<TextureBase>::GetAsset("checkbox1")
		);
	});

	menu.AddButton(0.66f, -0.69f, 0.02f, 0.04f, AssetLibrary<TextureBase>::GetAsset("plus"), [&](Button& button) {
		std::cout << "plus button clicked\n";
		optionManager.SetVolumeUp(!optionManager.GetVolumeUp());
		up = optionManager.GetUpTimes();
		up++;
		optionManager.SetUpTimes(up);
	});

	menu.AddButton(0.73f, -0.69f, 0.02f, 0.04f, AssetLibrary<TextureBase>::GetAsset("minus"), [&](Button& button) {
		std::cout << "minus button clicked\n";
		optionManager.SetVolumeDown(!optionManager.GetVolumeDown());
		down = optionManager.GetDownTimes();
		down++;
		optionManager.SetDownTimes(down);
	});
}