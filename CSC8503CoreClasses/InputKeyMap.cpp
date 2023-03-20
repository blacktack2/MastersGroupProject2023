#pragma once
#include "InputKeyMap.h"
#include "../CSC8503/GameStateManager.h"

#ifdef _ORBIS
#include "Vector2.h"
#include "../Plugins/PlayStation4/PS4InputManager.h"
#endif // _ORBIS


InputKeyMap::InputKeyMap() {
	XboxControllerManager::GetXboxController().CheckPorts();
	int numOfPlayers = XboxControllerManager::GetXboxController().GetActiveControllerNumber();
	if (numOfPlayers >= 4)	numOfPlayers = 4;
	for (int i = 1; i <= numOfPlayers; i++) {
		ChangePlayerControlTypeMap(i, ControllerType::Xbox);
	}
	buttonstates = InputType::Empty;
	movementAxis = NCL::Maths::Vector2(0);
	cameraAxis = NCL::Maths::Vector2(0);
	mousePosition = NCL::Maths::Vector2(0);
#ifdef x64
	ChangePlayerControlTypeMap(0, ControllerType::KeyboardMouse);
#endif // x64
#ifdef _ORBIS
	ChangePlayerControlTypeMap(0, ControllerType::PS4);
#endif // _ORBIS
}
InputKeyMap::~InputKeyMap() {}

void InputKeyMap::Update() {
	unsigned int oldStates = buttonstates;
	buttonstates = InputType::Empty;
	movementAxis = NCL::Maths::Vector2(0);
	cameraAxis = NCL::Maths::Vector2(0);

#ifdef _ORBIS
	PS4::PS4InputManager::Update();
#endif // _ORBIS

	for (auto playerTypePair : playerControlTypeMap) {
		UpdatePlayer(playerTypePair.first);
	}
	int numOfPlayers = XboxControllerManager::GetXboxController().GetActiveControllerNumber();
	if (numOfPlayers >= 4)	numOfPlayers = 4;
	for (int i = 1; i <= numOfPlayers; i++) {
		XboxControllerManager::GetXboxController().UpdateALastState(i);
	}

	upStates   = oldStates & (buttonstates ^ oldStates);
	downStates = buttonstates & (oldStates ^ buttonstates);
}

bool InputKeyMap::GetButton(InputType key, int PlayerID) {
	return CheckButtonPressed(buttonstates, key, PlayerID);
}

unsigned int InputKeyMap::GetButtonState() {
	return buttonstates;
}

bool InputKeyMap::CheckButtonPressed(unsigned int state, InputType key, int PlayerID) {

	if (key < Start) {
		return state & (key << (4 * PlayerID));
	}
	return state & key;
}

bool InputKeyMap::GetAxisData(unsigned int playerNum, AxisInput axis, float& data)
{
	if (axis == AxisInputDataMax) {
		return false;
	}
	if ((playerNum > 4))
	{
		return false;
	}
	data = AxisDataArray[playerNum][axis];
	return true;
}
#ifdef x64
Vector2 InputKeyMap::GetMousePosition() {
	return mousePosition;
}
bool InputKeyMap::HasMouse() {
	return Window::GetMouse();
}
#endif // x64


void InputKeyMap::ChangePlayerControlTypeMap(int playerID, ControllerType type)
{	
	playerControlTypeMap[playerID] = type;
}

void InputKeyMap::SetButton(InputType key, int PlayerID = 0)
{
	if (key < Start) {
		buttonstates |= (key << (4 * PlayerID));
	}
	else {
		buttonstates |= key;
	}
	
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
		UpdateWindowsGameStateDependant(playerID);
		break;
	case ControllerType::Xbox:
		UpdateXbox(playerID);
		UpdateXboxGameStateDependant(playerID);
		break;
	default:
		break;
	}
#endif // x64

#ifdef _ORBIS
	UpdatePS4(playerID);
	UpdatePS4GameStateDependant(playerID);
#endif // _ORBIS

	
	

}
#ifdef x64
void InputKeyMap::UpdateWindows(int playerID)
{
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W))
	{
		AxisDataArray[playerID][AxisInput::Axis2] = 1;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S))
	{
		AxisDataArray[playerID][AxisInput::Axis2] = -1;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A))
	{
		AxisDataArray[playerID][AxisInput::Axis1] = -1;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D))
	{
		AxisDataArray[playerID][AxisInput::Axis1] = 1;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SPACE))
	{
		SetButton(InputType::Jump, playerID);
		AxisDataArray[playerID][AxisInput::Axis6] = 1;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::C))
	{
		SetButton(InputType::FreeLook, playerID);
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::ESCAPE))
	{
		SetButton(InputType::Pause, playerID);
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN ) || Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT))
	{
		SetButton(InputType::DOWN, playerID);
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP) || Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT))
	{
		SetButton(InputType::UP, playerID);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::RETURN))
	{
		SetButton(InputType::Confirm, playerID);
	}

	if (Window::GetMouse()) {
		if (Window::GetMouse()->ButtonDown(MouseButtons::LEFT))
		{
			SetButton(InputType::Action1, playerID);
			AxisDataArray[playerID][AxisInput::Axis5] = 1;
		}
		if (Window::GetMouse()->ButtonPressed(MouseButtons::RIGHT))
		{
			SetButton(InputType::Action2, playerID);
		}

		if (Window::GetMouse()->ButtonPressed(MouseButtons::LEFT))
		{
			SetButton(InputType::Confirm);
		}

		AxisDataArray[playerID][AxisInput::Axis4] = -1.0f * Window::GetMouse()->GetRelativePosition().y;
		AxisDataArray[playerID][AxisInput::Axis3] = Window::GetMouse()->GetRelativePosition().x;

		mousePosition = Window::GetMouse()->GetAbsolutePosition();
	}
}

void InputKeyMap::UpdateWindowsGameStateDependant(int playerID) {
	GameStateManager* gameStateManager = &GameStateManager::instance();
	GameState gameState = gameStateManager->GetGameState();

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)) {
		SetButton(InputType::Pause);
	}

	switch (gameState) {
	case GameState::Lobby:
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::R)) {
			SetButton(InputType::Start);
		}
		break;
	case GameState::Win:
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::R)) {
			SetButton(InputType::Restart);
		}
		break;
	case GameState::Lose:
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::R)) {
			SetButton(InputType::Restart);
		}
		break;
	}

}

void InputKeyMap::UpdateXbox(int playerID)
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

		if (AxisDataArray[playerID][AxisInput::Axis2] > 0) {
			SetButton(InputType::UP);
		}
		if (AxisDataArray[playerID][AxisInput::Axis2] < 0) {
			SetButton(InputType::DOWN);
		}
	}
	if (XboxControllerManager::GetXboxController().GetThumbRight(playerID, thumbRight))
	{
		AxisDataArray[playerID][AxisInput::Axis3] = thumbRight.x;
		AxisDataArray[playerID][AxisInput::Axis4] = thumbRight.y;
		if (AxisDataArray[playerID][AxisInput::Axis4] > 0) {
			SetButton(InputType::UP);
		}
		if (AxisDataArray[playerID][AxisInput::Axis4] < 0) {
			SetButton(InputType::DOWN);
		}
	}
	if (XboxControllerManager::GetXboxController().GetRightTrigger(playerID, rightTriggerDepth))
	{
		AxisDataArray[playerID][AxisInput::Axis5] = rightTriggerDepth;
	}
	if (XboxControllerManager::GetXboxController().GetLeftTrigger(playerID, leftTriggerDepth))
	{
		AxisDataArray[playerID][AxisInput::Axis6] = leftTriggerDepth;
	}
	if (XboxControllerManager::GetXboxController().GetButton(playerID, XINPUT_GAMEPAD_X)) {
		SetButton(InputType::Jump, playerID);
	}
	if (XboxControllerManager::GetXboxController().GetButton(playerID, XINPUT_GAMEPAD_DPAD_UP)) {
		SetButton(InputType::UP);
	}
	if (XboxControllerManager::GetXboxController().GetButton(playerID, XINPUT_GAMEPAD_DPAD_DOWN)) {
		SetButton(InputType::DOWN);
	}
	if (XboxControllerManager::GetXboxController().GetButton(playerID, XINPUT_GAMEPAD_A)) {
		SetButton(InputType::Confirm);
	}
}

void InputKeyMap::UpdateXboxGameStateDependant(int playerID) {
	GameStateManager* gameStateManager = &GameStateManager::instance();
	GameState gameState = gameStateManager->GetGameState();

	if (XboxControllerManager::GetXboxController().GetButton(playerID, XINPUT_GAMEPAD_BACK)) {
		SetButton(InputType::Pause);
	}
	switch (gameState) {
	case GameState::Lobby:
		if (XboxControllerManager::GetXboxController().GetButton(playerID, XINPUT_GAMEPAD_START)) {
			SetButton(InputType::Start);
		}
		break;
	case GameState::Win:
		if (XboxControllerManager::GetXboxController().GetButton(playerID, XINPUT_GAMEPAD_START)) {
			SetButton(InputType::Restart);
		}
		break;
	case GameState::Lose:
		if (XboxControllerManager::GetXboxController().GetButton(playerID, XINPUT_GAMEPAD_START)) {
			SetButton(InputType::Restart);
		}
		break;
	}

}
#endif // x64

#ifdef _ORBIS
void NCL::InputKeyMap::UpdatePS4(int playerID) {
	//Axis
	float leftTriggerDepth;
	if (PS4::PS4InputManager::GetAxis(playerID, PS4::LEFTSTICK) != Maths::Vector2(-2))
	{
		Maths::Vector2 temp = PS4::PS4InputManager::GetAxis(playerID, PS4::LEFTSTICK);
		AxisDataArray[playerID][AxisInput::Axis1] = temp.x;
		AxisDataArray[playerID][AxisInput::Axis2] = temp.y;
	}
	if (PS4::PS4InputManager::GetAxis(playerID, PS4::RIGHTSTICK) != Maths::Vector2(-2))
	{
		Maths::Vector2 temp = PS4::PS4InputManager::GetAxis(playerID, PS4::RIGHTSTICK);
		AxisDataArray[playerID][AxisInput::Axis3] = temp.x;
		AxisDataArray[playerID][AxisInput::Axis4] = temp.y;
	}
	if (PS4::PS4InputManager::GetAxis(playerID, PS4::KEYPAD) != Maths::Vector2(-2))
	{
		Maths::Vector2 temp = PS4::PS4InputManager::GetAxis(playerID, PS4::KEYPAD);
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


void InputKeyMap::UpdatePS4GameStateDependant(int playerID) {
	NCL::CSC8503::GameStateManager* gameStateManager = &NCL::CSC8503::GameStateManager::instance();
	GameState gameState = gameStateManager->GetGameState();
	switch (gameState) {
	case GameState::Lobby:
			if (PS4::PS4InputManager::GetButtons(playerID, PS4::CROSS)) {
				SetButton(InputType::Start);
			}
	case GameState::OnGoing: default:
			if (PS4::PS4InputManager::GetButtons(playerID, PS4::START)) {
				SetButton(InputType::Pause);
			}
		break;
	case GameState::Win:
			if (PS4::PS4InputManager::GetButtons(playerID, PS4::START)) {
				SetButton(InputType::Restart);
			}
		break;
	case GameState::Lose:
			if (PS4::PS4InputManager::GetButtons(playerID, PS4::START)) {
				SetButton(InputType::Restart);
			}
		break;
	}
}

#endif // _ORBIS