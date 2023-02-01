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

		void Update() {
			buttonstates = InputType::Empty;
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
			if (Window::GetMouse()->ButtonPressed(MouseButtons::LEFT))
			{
				buttonstates |= InputType::Action1;
			}
			if (Window::GetMouse()->ButtonPressed(MouseButtons::RIGHT))
			{
				buttonstates |= InputType::Action2;
			}
			if (Window::GetKeyboard()->KeyDown(KeyboardKeys::C))
			{
				buttonstates |= InputType::FreeLook;
			}
		}
	private:
		InputKeyMap(){}
		~InputKeyMap(){}
		unsigned int buttonstates;

		

	};
}
