/**
 * @file   InputDevice.h
 * @brief  Base class for retrieving and handling user input from the OS.
 * 
 * @author Rich Davison
 * @author Stuart Lewis
 * @date   January 2023
 */
#pragma once
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC ((USHORT)0x01)
#endif

#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE ((USHORT)0x02)
#endif

#ifndef HID_USAGE_GENERIC_KEYBOARD
#define HID_USAGE_GENERIC_KEYBOARD ((USHORT)0x06)
#endif

#include <Windows.h>

namespace paintHell::core::system {
	/**
	 * @brief Base class for retrieving and handling user input from the OS.
	 */
	class InputDevice {
	protected:
		friend class Window;
		InputDevice() : mIsAwake(true), mRid() {}
		~InputDevice() {}

		/**
		 * @brief Update the state of the input source represented by raw.
		 */
		virtual void update(RAWINPUT* raw) = 0;
		/**
		 * @brief Update the hold state of each input source.
		 */
		virtual void updateHolds() {}
		/**
		 * @brief Disable this input device, preventing reactions to any
		 * input source.
		 */
		virtual void sleep() { mIsAwake = false; }
		/**
		 * @brief Re-enable this input device, allowing reactions to input
		 * sources.
		 */
		virtual void wake() { mIsAwake = true; }

		bool mIsAwake;
		RAWINPUTDEVICE mRid;
	};
}
