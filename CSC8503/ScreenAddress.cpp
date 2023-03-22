/**
 * @file   ScreenAddress.cpp
 * @brief  See ScreenAddress.h.
 *
 * @author Felix Chiu
 * @date   March 2023
 */
#pragma once
#include "ScreenAddress.h"
#include "AssetLibrary.h"

using namespace NCL;
using namespace CSC8503;

ScreenAddress::ScreenAddress() {
	InitMenu();
	OnAwake();
}

ScreenAddress::~ScreenAddress() {
	menuManager.RemoveAndEraseMenu(NAME);
}

PushdownState::PushdownResult ScreenAddress::OnUpdate(float dt, PushdownState** newState) {
	std::string address;
	for (int i = 0; i < 4; i++) {
		address += std::to_string(optionManager.GetIpAddress().GetAddress(i));
		if (i != 3) {
			address += ".";
		}
	}
	Debug::Print(address, Vector2(50.0f, 40.0f), Debug::BLUE);
	std::cout << address << std::endl;
	menuManager.Update(dt);
	keyMap.Update();
	renderer.Render();
	if (menuState == ChangeState::Resume || menuState == ChangeState::Quit) {
		return PushdownResult::Pop;
	}
	menuState = ChangeState::OnGoing;
	Debug::UpdateRenderables(dt);
	return PushdownResult::NoChange;
}

void ScreenAddress::OnAwake() {
	menuState = ChangeState::None;

	menuManager.SetCurrentMenu(NAME);

	renderer.EnableOverlayPass("Menu", true);
	renderer.UpdatePipeline();

	optionManager.SetCounter(0);

	Window::GetWindow()->ShowOSPointer(true);
	Window::GetWindow()->LockMouseToWindow(false);
}

void ScreenAddress::InitMenu() {
	Menu& menu = menuManager.AddMenu(NAME, Vector2(0.0f), Vector2(1.0f), AssetLibrary<TextureBase>::GetAsset("menuMain"));

	for (int i = 0; i < 10; i++) {
		std::string asset = "num" + std::to_string(i);
		menu.AddButton(0.0f + 0.08f * i, 0.0f, 0.04f, 0.04f, AssetLibrary<TextureBase>::GetAsset(asset), [&, i, asset](Button& button) {
			std::cout << "num " << i << " clicked\n";
			optionManager.GetIpAddress().AddDigit(i);
		}, [&, i, asset](Button& button) {
			num[i] = false;
			if (optionManager.GetCounter() % NUMBTN == i+1) {
				std::cout << asset << " selected\n";
				num[i] = true;
			}
			num[i] ? button.SetTexture(AssetLibrary<TextureBase>::GetAsset("plus")) : button.SetTexture(AssetLibrary<TextureBase>::GetAsset(asset));
		}, [&, i, asset](Button& button) {
			if (num[i]) {
				std::cout << asset << " pressed\n";
			}
			num[i] = false;
			button.SetTexture(AssetLibrary<TextureBase>::GetAsset(asset));
		});
	}
	int i = 10;
	menu.AddButton(0.0f + 0.08f * i, 0.0f, 0.04f, 0.04f, AssetLibrary<TextureBase>::GetAsset("dot"), [&, i](Button& button) {
		std::cout << "dot clicked\n";
		optionManager.GetIpAddress().NextIndex();
	}, [&, i](Button& button) {
		num[i] = false;
		if (optionManager.GetCounter() % NUMBTN == i+1) {
			std::cout << "dot selected\n";
			num[i] = true;
		}
		num[i] ? button.SetTexture(AssetLibrary<TextureBase>::GetAsset("plus")) : button.SetTexture(AssetLibrary<TextureBase>::GetAsset("dot"));
	}, [&, i](Button& button) {
		if (num[i]) {
			std::cout << "dot pressed\n";
		}
		num[i] = false;
		button.SetTexture(AssetLibrary<TextureBase>::GetAsset("dot"));
	});
	i = 11;
	menu.AddButton(0.0f + 0.08f * i, 0.0f, 0.04f, 0.04f, AssetLibrary<TextureBase>::GetAsset("backspace"), [&, i](Button& button) {
		std::cout << "backspace clicked\n";
		optionManager.GetIpAddress().RemoveDigit();
	}, [&, i](Button& button) {
		num[i] = false;
		if (optionManager.GetCounter() % NUMBTN == i+1) {
			std::cout << "backspace selected\n";
			num[i] = true;
		}
		num[i] ? button.SetTexture(AssetLibrary<TextureBase>::GetAsset("plus")) : button.SetTexture(AssetLibrary<TextureBase>::GetAsset("backspace"));
	}, [&, i](Button& button) {
		if (num[i]) {
			std::cout << "backspace pressed\n";
		}
		num[i] = false;
		button.SetTexture(AssetLibrary<TextureBase>::GetAsset("backspace"));
	});
	i = 12;
	menu.AddButton(0.5f, -0.6f, 0.2f, 0.1f, AssetLibrary<TextureBase>::GetAsset("button3"), [&, i](Button& button) {
		std::cout << "Quit button clicked\n";
		menuState = ChangeState::Quit;
	}, [&, i](Button& button) {
		num[i] = false;
		if (optionManager.GetCounter() % NUMBTN == 0) {
			std::cout << "Quit button selected\n";
			num[i] = true;
		}
		num[i] ? button.SetTexture(AssetLibrary<TextureBase>::GetAsset("buttonSlide3")) : button.SetTexture(AssetLibrary<TextureBase>::GetAsset("button3"));
	}, [&, i](Button& button) {
		if (num[i]) {
			std::cout << "Quit button pressed\n";
			menuState = ChangeState::Quit;
		}
		num[i] = false;
		button.SetTexture(AssetLibrary<TextureBase>::GetAsset("button3"));
	});
}