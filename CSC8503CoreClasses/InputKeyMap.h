#pragma once
#ifdef x64
#include "Window.h"
#include "XboxControllerManager.h"

#endif // x64

#ifdef _ORBIS
#include <limits>
#include <map>
#include "../NCLCoreClasses/Vector2.h"
#endif // _ORBIS

constexpr auto MAXPLAYER = 4;

enum InputType : unsigned int {
	Empty = 0,

	Jump = (1u << 0),
	Action1 = (1u << 1),
	Action2 = (1u << 2),
	FreeLook = (1u << 3),

	Jump_2 = (1u << 4),
	Action1_2 = (1u << 5),
	Action2_2 = (1u << 6),
	FreeLook_2 = (1u << 7),

	Jump_3 = (1u << 8),
	Action1_3 = (1u << 9),
	Action2_3 = (1u << 10),
	FreeLook_3 = (1u << 11),

	Jump_4 = (1u << 12),
	Action1_4 = (1u << 13),
	Action2_4 = (1u << 14),
	FreeLook_4 = (1u << 15),

	DOWN = (1u << 25),
	UP = (1u << 26),

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

using namespace NCL;

namespace NCL {
	class InputKeyMap {
	public:
		static InputKeyMap& instance() {
			static InputKeyMap INSTANCE;
			return INSTANCE;
		}

		void Update();

		bool GetButtonUp(InputType key) {
			return upStates & key;
		}

		bool GetButtonDown(InputType key) {
			return downStates & key;
		}

		bool GetButton(InputType key, int PlayerID = 0);

		unsigned int GetButtonState();

		bool CheckButtonPressed(unsigned int state, InputType key, int PlayerID);

		bool GetAxisData(unsigned int playerNum, AxisInput axis, float& data);
#ifdef x64
		Vector2 GetMousePosition();
		bool HasMouse();
#endif // x64


		void ChangePlayerControlTypeMap(int playerID, ControllerType type);

	private:
		InputKeyMap();
		~InputKeyMap();

		void SetButton(InputType key, int PlayerID);

		void UpdatePlayer(int playerID);

#ifdef x64
		void UpdateWindows(int playerID);

		void UpdateWindowsGameStateDependant(int playerID);

		void UpdateXbox(int playerID);
		void UpdateXboxGameStateDependant(int playerID);
#endif // x64

#ifdef _ORBIS
		void UpdatePS4(int playerID);
#endif // _ORBIS

		unsigned int buttonstates;
		float AxisDataArray[MAXPLAYER][AxisInput::AxisInputDataMax] = { 0 };
		unsigned int upStates;
		unsigned int downStates;
		Maths::Vector2 movementAxis;
		Maths::Vector2 cameraAxis;
		Maths::Vector2 mousePosition;

		std::map<int, ControllerType> playerControlTypeMap;
	};
}