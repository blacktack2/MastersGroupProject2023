/**
 * @file   Mouse.h
 * @brief  
 * 
 * @author Rich Davison
 * @author Stuart Lewis
 * @date   January 2023
 */
#pragma once
#include "InputDevice.h"

namespace Graphics {
	namespace Application {
		enum class MouseButton {
			MOUSE_LEFT = 0,
			MOUSE_RIGHT,
			MOUSE_MIDDLE,
			MOUSE_FOUR,
			MOUSE_FIVE,
			MOUSE_MAX,
		};

		struct MPos {
			MPos() : x(0.0f), y(0.0f) {}
			MPos(LONG x, LONG y) : x(x), y(y) {}
			LONG x, y;
		};

		class Mouse : public InputDevice {
		public:
			friend class Window;

			bool isButtonDown(MouseButton button);
			bool isButtonHeld(MouseButton button);
			bool isButtonDoubleClicked(MouseButton button);

			int getMouseWheelMovement();

			MPos getMousePosRelative();
			MPos getMousePosAbsolute();

			void setDoubleClickLimit(float msec);
			void setMouseSensitivity(float sensitivity);
		protected:
			Mouse(HWND& hwnd);
			~Mouse();

			virtual void update(RAWINPUT* raw);
			virtual void updateHolds();
			virtual void sleep();

			void updateDoubleClick(float dt);

			void setAbsolutePosition(LONG x, LONG y);
			void setAbsolutePositionBounds(LONG maxX, LONG maxY);

			MPos mAbsolutePosition;
			MPos mAbsolutePositionBounds;
			MPos mRelativePosition;
			
			bool mButtons[(size_t)MouseButton::MOUSE_MAX];
			bool mHoldButtons[(size_t)MouseButton::MOUSE_MAX];
			bool mDoubleClicks[(size_t)MouseButton::MOUSE_MAX];
			float mLastClickTime[(size_t)MouseButton::MOUSE_MAX];

			int mLastWheelTime;
			int mFrameWheel;

			float mClickLimit;
			float mSensitivity;

			bool mSetAbsolute;
		};
	}
}
