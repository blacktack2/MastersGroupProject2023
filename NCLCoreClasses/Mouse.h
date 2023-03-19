/**
 * @file   Mouse.h
 * @brief  Handler class for system mouse input.
 * 
 * @author Rich Davidson
 * @date   March 2023
 */
#pragma once
#include "Vector2.h"

namespace NCL {
	using namespace NCL::Maths;
	enum class MouseButtons {
		LEFT   = 0,
		RIGHT  = 1,
		MIDDLE = 2,
		FOUR   = 3,
		FIVE   = 4,
		MAXVAL = 5,
	};

	/**
	 * @brief Handler class for system mouse input.
	 */
	class Mouse {
	public:
		friend class Window;
		/**
		 * @return true if button has been pressed this frame, otherwise false.
		 */
		inline bool ButtonPressed(MouseButtons button) const {
			return buttons[(int)button] && !holdButtons[(int)button];
		}
		/**
		 * @return true if button is currently pressed, otherwise false.
		 */
		inline bool ButtonDown(MouseButtons button) const {
			return buttons[(int)button];
		}
		/**
		 * @return true if button has been held down for multiple frames,
		 * otherwise false.
		 */
		inline bool ButtonHeld(MouseButtons button) const {
			return buttons[(int)button] && holdButtons[(int)button];
		}
		/**
		 * @return true if two mouse clicks have been received in a short
		 * time span, otherwise false (see Mouse::SetDoubleClickLimit()).
		 */
		inline bool DoubleClicked(MouseButtons button) const {
			return (buttons[(int)button] && doubleClicks[(int)button]);
		}

		/**
		 * @return How much the mouse has moved since last frame
		 */
		inline Vector2 GetRelativePosition() const {
			return relativePosition;
		}
		/**
		 * @return Absolution position of the mouse pointer on the window.
		 */
		inline Vector2 GetAbsolutePosition() const {
			return absolutePosition;
		}

		inline Vector2 GetWindowPosition() const {
			return windowPosition;
		}

		inline bool WheelMoved() const {
			return frameWheel != 0;
		}
		/**
		 * @returns Positive if receiving scroll up input, negative if
		 * receiving scroll down input, 0 if no scroll input is received.
		 */
		inline int GetWheelMovement() const {
			return (int)frameWheel;
		}

		/**
		 * @brief Set the mouse sensitivity.
		 * @brief Currently only affects the 'relative' (i.e FPS-style) mouse
		 * movement.
		 */
		inline void SetMouseSensitivity(float amount) {
			if (amount == 0.0f) {
				amount = 1.0f;
			}
			sensitivity = amount;
		}

		/**
		 * @brief Set the maximum amount of ms that can pass between
		 * 2 mouse presses while still counting as a 'double click'.
		 */
		void SetDoubleClickLimit(float msec) {
			clickLimit = msec;
		}
	protected:
		Mouse();
		virtual ~Mouse() = default;

		void SetAbsolutePosition(const Vector2& pos);

		/**
		 * @brief Set the absolute screen bounds.
		 * @brief < 0 is always assumed dissallowed.
		 */
		void SetAbsolutePositionBounds(const Vector2& bounds);

		void UpdateFrameState(float msec);
		void Sleep();
		void Wake();

		bool isAwake;

		Vector2 windowPosition;
		Vector2 absolutePosition;
		Vector2 absolutePositionBounds;
		Vector2 relativePosition;
		bool buttons[(int)MouseButtons::MAXVAL];
		bool holdButtons[(int)MouseButtons::MAXVAL];
		bool doubleClicks[(int)MouseButtons::MAXVAL];
		float lastClickTime[(int)MouseButtons::MAXVAL];

		int lastWheel;

		int frameWheel;

		float clickLimit;

		float sensitivity;

		float m_fPosX;
		float m_fPosY;
		float m_fWidth;
		float m_fHeight;

		float g_fWidth = 1280;
		float g_fHeight = 720;
	};
}
