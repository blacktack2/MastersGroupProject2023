#pragma once
#ifdef x64
#include "Window.h"
#include "XboxControllerManager.h"

#endif // x64

#ifdef _ORBIS
#include <limits>
#include <map>
#include "../Common/Vector2.h"
#endif // _ORBIS


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

enum ControllerType {
	KeyboardMouse,
	Xbox,
	PS4
};



namespace NCL {
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

		bool GetAxisData(unsigned int playerNum, AxisInput axis, float& data)		// controllerNum == 1,2,3,4,5
		{
			if ((playerNum > 4))
			{
				return false;
			}
			data = AxisDataArray[playerNum][axis];
			return true;
		}
#ifdef x64
		Vector2 GetMousePosition() {
			return mousePosition;
	}
		bool HasMouse() {
			return Window::GetMouse();
		}
#endif // x64

		
		void Update();

		void ChangePlayerControlTypeMap(int playerID, ControllerType type);

	private:
		InputKeyMap() {
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
		~InputKeyMap() {}

		void UpdateGameStateDependant();

		void UpdatePlayer(int playerID);
#ifdef x64
		void UpdateWindows(int playerID);

		void UpdateXbox(int playerID);
#endif // x64

#ifdef _ORBIS
		void UpdatePS4(int playerID);
#endif // _ORBIS


		unsigned int buttonstates;
		float AxisDataArray[5][AxisInput::AxisInputDataMax] = { 0 };
		NCL::Maths::Vector2 movementAxis;
		NCL::Maths::Vector2 cameraAxis;
		NCL::Maths::Vector2 mousePosition;

		std::map<int, ControllerType> playerControlTypeMap;
	};
}
