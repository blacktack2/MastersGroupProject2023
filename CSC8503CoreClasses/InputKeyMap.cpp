#pragma once
#include "InputKeyMap.h"
#include "../CSC8503/GameStateManager.h"

using namespace NCL;
using namespace CSC8503;
using namespace NCL;

void InputKeyMap::Update() {
	unsigned int oldStates = buttonstates;
	buttonstates = InputType::Empty;
	movementAxis = Vector2(0);
	cameraAxis = Vector2(0);

	for (auto playerTypePair : playerControlTypeMap) {
		UpdatePlayer(playerTypePair.first);
	}

	upStates   = oldStates & (buttonstates ^ oldStates);
	downStates = buttonstates & (oldStates ^ buttonstates);
}

void InputKeyMap::ChangePlayerControlTypeMap(int playerID, ControllerType type)
{	
	playerControlTypeMap[playerID] = type;
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

void InputKeyMap::UpdatePlayer(int playerID)
{
	for (int j = 0; j < AxisInput::AxisInputDataMax; j++)
	{
		AxisDataArray[playerID][j] = 0.0f;
	}
	switch (playerControlTypeMap[playerID])
	{
	case ControllerType::KeyboardMouse:
		UpdateWindows(playerID);
		break;
	case ControllerType::Xbox:
		UpdateXbox(playerID);
		break;
	default:
		break;
	}
	UpdateGameStateDependant();

}

void NCL::InputKeyMap::UpdateWindows(int playerID)
{
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W))
	{
		buttonstates |= InputType::Foward;
		AxisDataArray[playerID][AxisInput::Axis2] = 1;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S))
	{
		buttonstates |= InputType::Backward;
		AxisDataArray[playerID][AxisInput::Axis2] = -1;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A))
	{
		buttonstates |= InputType::Left;
		AxisDataArray[playerID][AxisInput::Axis1] = -1;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D))
	{
		buttonstates |= InputType::Right;
		AxisDataArray[playerID][AxisInput::Axis1] = 1;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SPACE))
	{
		buttonstates |= InputType::Jump;
		AxisDataArray[playerID][AxisInput::Axis6] = 1;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::C))
	{
		buttonstates |= InputType::FreeLook;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::ESCAPE))
	{
		buttonstates |= InputType::ESC;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN ) || Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT))
	{
		buttonstates |= InputType::DOWN;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP) || Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT))
	{
		buttonstates |= InputType::UP;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RETURN))
	{
		buttonstates |= InputType::Confirm;
	}

	if (Window::GetMouse()) {
		if (Window::GetMouse()->ButtonDown(MouseButtons::LEFT))
		{
			buttonstates |= InputType::Action1;
			AxisDataArray[playerID][AxisInput::Axis5] = 1;
		}
		if (Window::GetMouse()->ButtonPressed(MouseButtons::RIGHT))
		{
			buttonstates |= InputType::Action2;
		}

		if (Window::GetMouse()->ButtonPressed(MouseButtons::LEFT))
		{
			buttonstates |= InputType::Confirm;
		}

		AxisDataArray[playerID][AxisInput::Axis4] = -1.0f * Window::GetMouse()->GetRelativePosition().y;
		AxisDataArray[playerID][AxisInput::Axis3] = Window::GetMouse()->GetRelativePosition().x;

		mousePosition = Window::GetMouse()->GetAbsolutePosition();
	}
}

void NCL::InputKeyMap::UpdateXbox(int playerID)
{
	// Axis:
	Vector2 thumbLeft;
	Vector2 thumbRight;
	float rightTriggerDepth;
	float leftTriggerDepth;
	if (XboxControllerManager::GetXboxController().GetThumbLeft(playerID, thumbLeft))
	{
		AxisDataArray[playerID][AxisInput::Axis1] = thumbLeft.x;
		AxisDataArray[playerID][AxisInput::Axis2] = thumbLeft.y;
	}
	if (XboxControllerManager::GetXboxController().GetThumbRight(playerID, thumbRight))
	{
		AxisDataArray[playerID][AxisInput::Axis3] = thumbRight.x;
		AxisDataArray[playerID][AxisInput::Axis4] = thumbRight.y;
	}
	if (XboxControllerManager::GetXboxController().GetRightTrigger(playerID, rightTriggerDepth))
	{
		AxisDataArray[playerID][AxisInput::Axis5] = rightTriggerDepth;
	}
	if (XboxControllerManager::GetXboxController().GetLeftTrigger(playerID, leftTriggerDepth))
	{
		AxisDataArray[playerID][AxisInput::Axis6] = leftTriggerDepth;
	}
}
