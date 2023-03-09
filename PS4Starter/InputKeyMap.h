#pragma once
#ifdef x64
#include "../Common/Window.h"
#endif //- x64

#include"../Common/Vector2.h"

enum InputType :unsigned int {
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

	Start = (1u << 27),
	Pause = (1u << 28),
	Restart = (1u << 29),
	Return = (1u << 30),
	Confirm = (1u << 31),

	All = UINT_MAX
};

enum AxisInput
{
	Axis1,
	Axis2,

	Axis3,
	Axis4,

	Axis5,
	Axis6,

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

#ifdef x64
		Maths::Vector2 GetMousePosition() {
			return mousePosition;
	}
		bool HasMouse() {
			return Window::GetMouse();
		}
#endif // x64

		void Update();

	private:
		InputKeyMap() {
			buttonstates = InputType::Empty;
			movementAxis = Maths::Vector2(0);
			cameraAxis = Maths::Vector2(0);
			mousePosition = Maths::Vector2(0);
		}
		~InputKeyMap() {}

		void UpdateGameStateDependant();

		unsigned int buttonstates;
		float AxisDataArray[4][AxisInput::AxisInputDataMax] = { 0 };
		Maths::Vector2 movementAxis;
		Maths::Vector2 cameraAxis;
		Maths::Vector2 mousePosition;


	};
}
