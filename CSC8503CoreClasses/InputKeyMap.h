#pragma once
#include "Window.h"

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
	
	MouseLeftClick = (1u << 12),
	MouseRightClick = (1u << 13),

	Restart = (1u << 20),
	Return = (1u << 21),
	Confirm = (1u << 22),

	All = 256
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

		Vector2 GetMousePosition() {
			return mousePosition;
		}
		bool HasMouse() {
			return Window::GetMouse();
		}

		void Update();

	private:
		InputKeyMap(){
			buttonstates = InputType::Empty;
			movementAxis = Vector2(0);
			cameraAxis = Vector2(0);
			mousePosition = Vector2(0);
		}
		~InputKeyMap(){}

		void UpdateGameStateDependant();

		unsigned int buttonstates;

		Vector2 movementAxis;
		Vector2 cameraAxis;
		Vector2 mousePosition;
	};
}
