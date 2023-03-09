#pragma once
#include "InputKeyMap.h"
#ifdef x64
	#include "../CSC8503/GameStateManager.h"
#endif // x64
#ifdef _ORBIS
#include "GameStateManager.h"
#include "../Plugins/PlayStation4/PS4InputManager.h"
#endif // _ORBIS


using namespace NCL;
using namespace CSC8503;
using namespace paintHell;

void InputKeyMap::Update() {
	buttonstates = InputType::Empty;
	movementAxis = Maths::Vector2(0);
	cameraAxis = Maths::Vector2(0);
	// Axis:
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < AxisInput::AxisInputDataMax; j++)
		{
			AxisDataArray[i][j] = 0.0f;
		}
	}

	Maths::Vector2 thumbLeft;
	Maths::Vector2 thumbRight;
	float rightTriggerDepth;
	float leftTriggerDepth;

#ifdef x64
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
#elif _ORBIS
	PS4::PS4InputManager::Update();
	for (int i = 0; i < SCE_USER_SERVICE_MAX_LOGIN_USERS; i++) {
		if (PS4::PS4InputManager::GetAxis(i, PS4::LEFTSTICK) != Maths::Vector2(Axis_Negative)) {
			Maths::Vector2 temp = PS4::PS4InputManager::GetAxis(i, PS4::LEFTSTICK);
			AxisDataArray[i][AxisInput::Axis1] = temp.x;
			AxisDataArray[i][AxisInput::Axis2] = temp.y;
		}
		if (PS4::PS4InputManager::GetAxis(i, PS4::KEYPAD) != Maths::Vector2(Axis_Negative)) {
			Maths::Vector2 temp = PS4::PS4InputManager::GetAxis(i, PS4::KEYPAD);
			AxisDataArray[i][AxisInput::Axis1] = temp.x;
			AxisDataArray[i][AxisInput::Axis2] = temp.y;
		}
		if (PS4::PS4InputManager::GetAxis(i, PS4::RIGHTSTICK) != Maths::Vector2(Axis_Negative)) {
			Maths::Vector2 temp = PS4::PS4InputManager::GetAxis(i, PS4::RIGHTSTICK);
			AxisDataArray[i][AxisInput::Axis3] = temp.x;
			AxisDataArray[i][AxisInput::Axis4] = temp.y;
		}

		if (PS4::PS4InputManager::GetButtons(i, PS4::R2)) {
			AxisDataArray[i][AxisInput::Axis5] = 1.0f;
			
		}
		if (PS4::PS4InputManager::GetButtons(i, PS4::L2)) {
			AxisDataArray[i][AxisInput::Axis6] = 1.0f;
			
		}

		if (PS4::PS4InputManager::GetButtons(i, PS4::START)) {
			buttonstates |= InputType::ESC;
		}



	}

#endif // x64

	

	UpdateGameStateDependant();

#ifdef x64
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
#endif // x64

	
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

#elif _ORBIS
	switch (gameState) {
	case GameState::Lobby:
		for (unsigned int i = 0; i < SCE_USER_SERVICE_MAX_LOGIN_USERS; i++) {
			if (PS4::PS4InputManager::GetButtons(i, PS4::CROSS)) {
				buttonstates |= InputType::Start;
				break;
			}
		}
		
	case GameState::OnGoing: default:
		for (unsigned int i = 0; i < SCE_USER_SERVICE_MAX_LOGIN_USERS; i++) {
			if (PS4::PS4InputManager::GetButtons(i, PS4::START)) {
				buttonstates |= InputType::Pause;
				break;
			}
		}
		break;
	case GameState::Win:
		for (unsigned int i = 0; i < SCE_USER_SERVICE_MAX_LOGIN_USERS; i++) {
			if (PS4::PS4InputManager::GetButtons(i, PS4::CROSS)) {
				buttonstates |= InputType::Restart;
				break;
			}
		}
		break;
	case GameState::Lose:
		for (unsigned int i = 0; i < SCE_USER_SERVICE_MAX_LOGIN_USERS; i++) {
			if (PS4::PS4InputManager::GetButtons(i, PS4::CROSS)) {
				buttonstates |= InputType::Restart;
				break;
			}
		}
		break;
	}
#endif // x64

}