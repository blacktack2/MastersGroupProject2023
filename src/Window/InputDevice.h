/**
 * @file   InputDevice.h
 * @brief  
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

namespace Graphics {
	namespace Application {
		class InputDevice {
		protected:
			friend class Window;
			InputDevice() : mIsAwake(true), mRid() {}
			~InputDevice() {}

			virtual void update(RAWINPUT* raw) = 0;
			virtual void UpdateHolds() {}
			virtual void sleep() { mIsAwake = false; }
			virtual void wake() { mIsAwake = true; }

			bool mIsAwake;
			RAWINPUTDEVICE mRid;
		};
	}
}
