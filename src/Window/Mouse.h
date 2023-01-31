/**
 * @file   Mouse.h
 * @brief  Handler class for mouse events.
 * 
 * @author Rich Davison
 * @author Stuart Lewis
 * @date   January 2023
 */
#pragma once
#include "InputDevice.h"

namespace paintHell::core::system {
	enum class MouseButton {
		MOUSE_LEFT = 0,
		MOUSE_RIGHT,
		MOUSE_MIDDLE,
		MOUSE_FOUR,
		MOUSE_FIVE,
		MOUSE_MAX,
	};

	/**
	 * @brief Handler class for mouse events.
	 */
	class Mouse : public InputDevice {
	public:
		friend class Window;

		/**
		 * @returns true if button was pressed this frame, otherwise false.
		 */
		bool isButtonDown(MouseButton button);
		/**
		 * @returns true if button is currently held down, otherwise false.
		 */
		bool isButtonHeld(MouseButton button);
		/**
		 * @returns true if button has double clicked this frame, otherwise
		 * false.
		 */
		bool isButtonDoubleClicked(MouseButton button);
			
		/**
		 * @returns 0 if the mouse wheel has not moved this frame, 1 if the
		 * mouse wheel has scrolled up, and -1 if the mouse wheel has
		 * scrolled down.
		 */
		int getMouseWheelMovement();

		/**
		 * @returns How much the mouse has moved since the previous frame.
		 */
		POINT getMousePosRelative();
		/**
		 * @returns Position of the mouse relative to the window.
		 */
		POINT getMousePosAbsolute();

		void setDoubleClickLimit(float msec);
		void setMouseSensitivity(float sensitivity);
	protected:
		Mouse(HWND& hwnd);
		~Mouse();

		virtual void update(RAWINPUT* raw) override;
		virtual void updateHolds() override;
		virtual void sleep() override;

		/**
		 * @brief Update double click timers for each button.
		 * @param dt Time since last frame (milliseconds).
		 */
		void updateDoubleClick(float dt);

		/**
		 * @param pos Position to force the mouse to on the window.
		 */
		void setAbsolutePosition(POINT pos);
		/**
		 * @param bounds Maximum bounds the mouse can move (usually the
		 * size of the window).
		 */
		void setAbsolutePositionBounds(POINT bounds);

		POINT mAbsolutePosition;
		POINT mAbsolutePositionBounds;
		POINT mRelativePosition;
			
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
