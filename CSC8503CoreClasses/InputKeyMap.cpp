#pragma once
#include "InputKeyMap.h"
#include "../CSC8503/GameStateManager.h"

using namespace NCL;
using namespace CSC8503;
using namespace paintHell;

void InputKeyMap::Update() {
	buttonstates = InputType::Empty;
	movementAxis = Vector2(0);
	cameraAxis = Vector2(0);

	UpdateGameStateDependant();

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W))
	{
		buttonstates |= InputType::Foward;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S))
	{
		buttonstates |= InputType::Backward;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A))
	{
		buttonstates |= InputType::Left;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D))
	{
		buttonstates |= InputType::Right;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SPACE))
	{
		buttonstates |= InputType::Jump;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::C))
	{
		buttonstates |= InputType::FreeLook;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::ESCAPE))
	{
		buttonstates |= InputType::ESC;
	}
	if (Window::GetMouse()) {
		if (Window::GetMouse()->ButtonDown(MouseButtons::LEFT))
		{
			buttonstates |= InputType::Action1;
		}
		if (Window::GetMouse()->ButtonPressed(MouseButtons::RIGHT))
		{
			buttonstates |= InputType::Action2;
		}

		if (Window::GetMouse()->ButtonPressed(MouseButtons::LEFT))
		{
			buttonstates |= InputType::MouseLeftClick;
		}
		if (Window::GetMouse()->ButtonPressed(MouseButtons::RIGHT))
		{
			buttonstates |= InputType::MouseRightClick;
		}
		mousePosition = Window::GetMouse()->GetAbsolutePosition();
	}
}

void InputKeyMap::UpdateGameStateDependant() {
	GameStateManager* gameStateManager = &GameStateManager::instance();
	GameState gameState = gameStateManager->GetGameState();
	switch (gameState) {
	case GameState::OnGoing: default:
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)) {
			buttonstates |= InputType::ESC;
		}
		break;
	}

}