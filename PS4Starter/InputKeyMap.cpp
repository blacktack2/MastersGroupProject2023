#pragma once
#include "InputKeyMap.h"
#ifdef x64
#include "../CSC8503/GameStateManager.h"
#endif // x64

#ifdef _ORBIS
#include "GameStateManager.h"
#include "../Common/Vector2.h"
#include "../Plugins/PlayStation4/PS4InputManager.h"
#endif // _ORBIS


using namespace NCL;
using namespace CSC8503;
using namespace NCL;

void InputKeyMap::Update() {
	buttonstates = InputType::Empty;
	movementAxis = Maths::Vector2(0);
	cameraAxis = Maths::Vector2(0);

#ifdef _ORBIS
	PS4::PS4InputManager::Update();
#endif // _ORBIS

	for (auto playerTypePair : playerControlTypeMap) {
		UpdatePlayer(playerTypePair.first);
	}
}

void InputKeyMap::ChangePlayerControlTypeMap(int playerID, ControllerType type)
{
	playerControlTypeMap[playerID] = type;
}

void InputKeyMap::UpdateGameStateDependant() {
	GameStateManager* gameStateManager = &GameStateManager::instance();
	GameState gameState = gameStateManager->GetGameState();
#ifdef x64
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
#endif // x64

#ifdef _ORBIS
	switch (gameState) {
	case GameState::Lobby:
		for (int i = 0; i < SCE_USER_SERVICE_MAX_LOGIN_USERS; i++) {
			if (PS4::PS4InputManager::GetButtons(i, PS4::CROSS)) {
				buttonstates |= InputType::Start;
				break;
			}
		}
	case GameState::OnGoing: default:
		for (int i = 0; i < SCE_USER_SERVICE_MAX_LOGIN_USERS; i++) {
			if (PS4::PS4InputManager::GetButtons(i, PS4::START)) {
				buttonstates |= InputType::Pause;
				break;
			}
		}
		break;
	case GameState::Win:
		for (int i = 0; i < SCE_USER_SERVICE_MAX_LOGIN_USERS; i++) {
			if (PS4::PS4InputManager::GetButtons(i, PS4::START)) {
				buttonstates |= InputType::Restart;
				break;
			}
		}
		break;
	case GameState::Lose:
		for (int i = 0; i < SCE_USER_SERVICE_MAX_LOGIN_USERS; i++) {
			if (PS4::PS4InputManager::GetButtons(i, PS4::START)) {
				buttonstates |= InputType::Restart;
				break;
			}
		}
		break;
	}
#endif // _ORBIS

	

}

void InputKeyMap::UpdatePlayer(int playerID)
{
	for (int j = 0; j < AxisInput::AxisInputDataMax; j++)
	{
		AxisDataArray[playerID][j] = 0.0f;
	}
#ifdef x64
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
#endif // x64
#ifdef _ORBIS
		UpdatePS4(playerID);
#endif // _ORBIS

	UpdateGameStateDependant();

}
#ifdef x64
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
#endif // x64

#ifdef _ORBIS
void NCL::InputKeyMap::UpdatePS4(int playerID) {
	//Axis
	Maths::Vector2 thumbLeft;
	Maths::Vector2 thumbRight;
	float rightTriggerDepth;
	float leftTriggerDepth;
	if (PS4::PS4InputManager::GetAxis(playerID,PS4::LEFTSTICK)!=Maths::Vector2(-2))
	{
		Maths::Vector2 temp = PS4::PS4InputManager::GetAxis(playerID,PS4::LEFTSTICK);
		AxisDataArray[playerID][AxisInput::Axis1] = temp.x;
		AxisDataArray[playerID][AxisInput::Axis2] = -temp.y;
	}
	if (PS4::PS4InputManager::GetAxis(playerID,PS4::RIGHTSTICK)!=Maths::Vector2(-2))
	{
		Maths::Vector2 temp = PS4::PS4InputManager::GetAxis(playerID,PS4::RIGHTSTICK);
		AxisDataArray[playerID][AxisInput::Axis3] = temp.x;
		AxisDataArray[playerID][AxisInput::Axis4] = -temp.y;
	}
	if (PS4::PS4InputManager::GetAxis(playerID,PS4::KEYPAD)!=Maths::Vector2(-2))
	{
		Maths::Vector2 temp = PS4::PS4InputManager::GetAxis(playerID,PS4::KEYPAD);
		AxisDataArray[playerID][AxisInput::Axis1] = temp.x;
		AxisDataArray[playerID][AxisInput::Axis2] = temp.y;
	}
	if (PS4::PS4InputManager::GetButtons(playerID, PS4::R2)) {
		AxisDataArray[playerID][AxisInput::Axis5] = 1.0f;
	}
	if (PS4::PS4InputManager::GetButtons(playerID, PS4::L2)) {
		AxisDataArray[playerID][AxisInput::Axis6] = 1.0f;
	}

}
#endif // _ORBIS


