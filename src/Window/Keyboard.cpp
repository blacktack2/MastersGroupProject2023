/**
 * @file   Keyboard.cpp
 * @brief  
 * 
 * @author Rich Davison
 * @author Stuart Lewis
 * @date   January 2023
 */
#include "Keyboard.h"

using namespace Graphics;
using namespace Application;

Keyboard::Keyboard(HWND& hwnd) {
	ZeroMemory(mKeyStates, (size_t)KeyboardKey::KEYBOARD_MAX * sizeof(bool));
	ZeroMemory(mHoldStates, (size_t)KeyboardKey::KEYBOARD_MAX * sizeof(bool));

	mRid.usUsagePage = HID_USAGE_PAGE_GENERIC;
	mRid.usUsage = HID_USAGE_GENERIC_KEYBOARD;
	mRid.dwFlags = RIDEV_INPUTSINK;
	mRid.hwndTarget = hwnd;
	RegisterRawInputDevices(&mRid, 1, sizeof(mRid));
}

Keyboard::~Keyboard() {

}

bool Keyboard::isKeyPressed(KeyboardKey key) {
	return mKeyStates[(size_t)key];
}

bool Keyboard::isKeyReleased(KeyboardKey key) {
	return false;
}

bool Keyboard::isKeyHeld(KeyboardKey key) {
	return isKeyPressed(key) && mHoldStates[(size_t)key];
}

bool Keyboard::isKeyTriggered(KeyboardKey key) {
	return false;
}

void Keyboard::updateHolds() {
	memcpy(mHoldStates, mKeyStates, (size_t)KeyboardKey::KEYBOARD_MAX * sizeof(bool));
}

void Keyboard::update(RAWINPUT* raw) {
	if (mIsAwake) {
		DWORD key = (DWORD)raw->data.keyboard.VKey;

		if (key < 0 || key > (size_t)KeyboardKey::KEYBOARD_MAX) {
			return;
		}

		mKeyStates[key] = !(raw->data.keyboard.Flags & RI_KEY_BREAK);
	}
}

void Keyboard::sleep() {
}
