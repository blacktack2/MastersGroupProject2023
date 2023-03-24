/**
 * @file   Win32Keyboard.cpp
 * @brief  See Win32Keyboard.h.
 * 
 * @author Rich Davidson
 * @date   March 2023
 */
#include "Win32Keyboard.h"

#ifdef _WIN32
using namespace NCL::Win32Code;

Win32Keyboard::Win32Keyboard(HWND &hwnd) {
	rid.usUsagePage = HID_USAGE_PAGE_GENERIC;
	rid.usUsage     = HID_USAGE_GENERIC_KEYBOARD;
	rid.dwFlags     = RIDEV_INPUTSINK;
	rid.hwndTarget  = hwnd;
	RegisterRawInputDevices(&rid, 1, sizeof(rid));

	isAwake = true;
}

/*
Updates the keyboard state with data received from the OS.
*/
void Win32Keyboard::UpdateRAW(RAWINPUT* raw) {
	if(isAwake) {
		DWORD key = (DWORD)raw->data.keyboard.VKey;

		if(key < 0 || key >= (unsigned long)KeyboardKeys::MAXVALUE) {
			return;
		}

		keyStates[key] = !(raw->data.keyboard.Flags & RI_KEY_BREAK);
	}
}

#endif //_WIN32
