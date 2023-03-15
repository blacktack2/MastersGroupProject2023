/**
 * @file   Win32Mouse.cpp
 * @brief  See Win32Mouse.h.
 * 
 * @author Rich Davidson
 * @date   March 2023
 */
#include "Win32Mouse.h"

#ifdef _WIN32
#include <algorithm>

using namespace NCL::Win32Code;

Win32Mouse::Win32Mouse(HWND &hwnd) {
	rid.usUsagePage = HID_USAGE_PAGE_GENERIC; 
	rid.usUsage     = HID_USAGE_GENERIC_MOUSE; 
	rid.dwFlags     = RIDEV_INPUTSINK;   
	rid.hwndTarget  = hwnd;
	RegisterRawInputDevices(&rid, 1, sizeof(rid));

	setAbsolute = false;
}

void Win32Mouse::UpdateRAW(RAWINPUT* raw) {
	if (!isAwake) {
		return;
	}
	bool virtualDesktop = (raw->data.mouse.usFlags & MOUSE_VIRTUAL_DESKTOP) > 0;
	bool isAbsolute     = (raw->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE) > 0;

	if (isAbsolute) {
		const int screenWidth  = GetSystemMetrics(virtualDesktop ? SM_CXVIRTUALSCREEN : SM_CXSCREEN);
		const int screenHeight = GetSystemMetrics(virtualDesktop ? SM_CYVIRTUALSCREEN : SM_CYSCREEN);

		Vector2 prevAbsolute = absolutePosition;
		absolutePosition.x = (raw->data.mouse.lLastX / (float)USHRT_MAX) * screenWidth;
		absolutePosition.y = (raw->data.mouse.lLastY / (float)USHRT_MAX) * screenHeight;

		if (setAbsolute) {
			relativePosition = (absolutePosition - prevAbsolute) * sensitivity;
		}
		setAbsolute = true;
	} else {
		relativePosition.x += ((float)raw->data.mouse.lLastX) * sensitivity;
		relativePosition.y += ((float)raw->data.mouse.lLastY) * sensitivity;

		absolutePosition.x += (float)raw->data.mouse.lLastX;
		absolutePosition.y += (float)raw->data.mouse.lLastY;

		absolutePosition.x = std::max(absolutePosition.x, 0.0f);
		absolutePosition.x = std::min(absolutePosition.x, absolutePositionBounds.x);

		absolutePosition.y = std::max(absolutePosition.y, 0.0f);
		absolutePosition.y = std::min(absolutePosition.y, absolutePositionBounds.y);
	}

	if (raw->data.mouse.usButtonFlags & RI_MOUSE_WHEEL) {
		frameWheel = raw->data.mouse.usButtonData == 120 ? 1 : -1;
	}

	static int buttondowns [5] = {
		RI_MOUSE_BUTTON_1_DOWN,
		RI_MOUSE_BUTTON_2_DOWN,
		RI_MOUSE_BUTTON_3_DOWN,
		RI_MOUSE_BUTTON_4_DOWN,
		RI_MOUSE_BUTTON_5_DOWN
	};

	static int buttonps [5] = {
		RI_MOUSE_BUTTON_1_UP,
		RI_MOUSE_BUTTON_2_UP,
		RI_MOUSE_BUTTON_3_UP,
		RI_MOUSE_BUTTON_4_UP,
		RI_MOUSE_BUTTON_5_UP
	};
		
	for (int i = 0; i < 5; ++i) {
		if (raw->data.mouse.usButtonFlags & buttondowns[i]) {
			buttons[i] = true;

			if (lastClickTime[i] > 0) {
				doubleClicks[i] = true;
			}

			lastClickTime[i] = clickLimit;
		} else if (raw->data.mouse.usButtonFlags & buttonps[i]) {
			buttons[i]     = false;
			holdButtons[i] = false;
		}
	}
}
#endif //_WIN32
