#pragma once
#include "Window.h"
#include "XboxControllerManager.h"

enum InputType : unsigned int {
	Empty = 0,
	Foward = (1u << 0),
	Backward = (1u << 1),
	Left = (1u << 2),
	Right = (1u << 3),
	Jump = (1u << 4),
	Action1 = (1u << 5),
	Action2 = (1u << 6),
	FreeLook = (1u << 7),
	ESC = (1u << 8),
	
	MouseLeftClick = (1u << 12),
	MouseRightClick = (1u << 13),

	All = 256
};

enum AxisInput
{
	Axis1,
	Axis2,

	Axis3,
	Axis4,

	Axis5,

	AxisInputDataMax
};

using namespace NCL;

namespace paintHell {
	class InputKeyMap {
	public:
		static InputKeyMap& instance() {
			static InputKeyMap INSTANCE;
			return INSTANCE;
		}

		bool GetButton(InputType key) {
			return buttonstates & key;
		}

		unsigned int GetButtonState() {
			return buttonstates;
		}

		bool CheckButtonPressed(unsigned int state, InputType key) {
			return state & key;
		}

		bool GetAxisData(unsigned int controllerNum, AxisInput axis, float& data)		// controllerNum == 1,2,3,4
		{
			if ((controllerNum == 0) || (controllerNum > 4))
			{
				return false;
			}
			controllerNum--;
			data = AxisDataArray[controllerNum][axis];
			return true;
		}

		Vector2 GetMousePosition() {
			return mousePosition;
		}
		bool HasMouse() {
			return Window::GetMouse();
		}

		void Update() {

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
			for (int i = 0; i < 4; i++)
			{
				if (XboxControllerManager::GetXboxController().GetThumbLeft(i, thumbLeft))
				{
					AxisDataArray[i][AxisInput::Axis1] = thumbLeft.x;
					AxisDataArray[i][AxisInput::Axis2] = thumbLeft.y;
				}
				if (XboxControllerManager::GetXboxController().GetThumbLeft(i, thumbRight))
				{
					AxisDataArray[i][AxisInput::Axis3] = thumbRight.x;
					AxisDataArray[i][AxisInput::Axis4] = thumbRight.y;
				}
				if (XboxControllerManager::GetXboxController().GetRightTrigger(i, rightTriggerDepth))
				{
					AxisDataArray[i][AxisInput::Axis5] = rightTriggerDepth;
				}
			}

			// Button:
			buttonstates = InputType::Empty;
			movementAxis = Vector2(0);
			cameraAxis = Vector2(0);

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

	private:
		InputKeyMap(){
			buttonstates = InputType::Empty;
			movementAxis = Vector2(0);
			cameraAxis = Vector2(0);
			mousePosition = Vector2(0);
		}
		~InputKeyMap(){}
		unsigned int buttonstates;
		float AxisDataArray[4][AxisInput::AxisInputDataMax];
		Vector2 movementAxis;
		Vector2 cameraAxis;
		Vector2 mousePosition;
	};
}
