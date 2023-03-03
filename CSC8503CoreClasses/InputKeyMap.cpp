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
	// Axis:
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < AxisInput::AxisInputDataMax; j++)
		{
			AxisDataArray[i][j] = 0.0f;
		}
	}

	Vector2 thumbLeft;
	Vector2 thumbRight;
	float rightTriggerDepth;
	float leftTriggerDepth;
	for (int i = 0; i < 4; i++)
	{
		if (XboxControllerManager::GetXboxController().GetThumbLeft(i, thumbLeft))
		{
			AxisDataArray[i][AxisInput::Axis1] = thumbLeft.x;
			AxisDataArray[i][AxisInput::Axis2] = thumbLeft.y;
		}
		if (XboxControllerManager::GetXboxController().GetThumbRight(i, thumbRight))
		{
			AxisDataArray[i][AxisInput::Axis3] = thumbRight.x;
			AxisDataArray[i][AxisInput::Axis4] = thumbRight.y;
		}
		if (XboxControllerManager::GetXboxController().GetRightTrigger(i, rightTriggerDepth))
		{
			AxisDataArray[i][AxisInput::Axis5] = rightTriggerDepth;
		}
		if (XboxControllerManager::GetXboxController().GetLeftTrigger(i, leftTriggerDepth))
		{
			AxisDataArray[i][AxisInput::Axis6] = leftTriggerDepth;
		}
	}

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
			buttonstates |= InputType::Confirm;
		}

		mousePosition = Window::GetMouse()->GetAbsolutePosition();
	}
}

void InputKeyMap::UpdateGameStateDependant() {
	GameStateManager* gameStateManager = &GameStateManager::instance();
	GameState gameState = gameStateManager->GetGameState();
	switch (gameState) {
	case GameState::Lobby:
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::R)) {
			buttonstates |= InputType::Start;
		}
	case GameState::OnGoing: default:
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)) {
			buttonstates |= InputType::Pause;
		}
		break;
	case GameState::Win:
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE)) {
			buttonstates |= InputType::Restart;
		}
		break;
	case GameState::Lose:
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE)) {
			buttonstates |= InputType::Restart;
		}
		break;
	}

}