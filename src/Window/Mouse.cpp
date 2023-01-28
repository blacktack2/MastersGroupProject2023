/**
 * @file   Mouse.cpp
 * @brief  See Mouse.h.
 * 
 * @author Rich Davison
 * @author Stuart Lewis
 * @date   January 2023
 */
#define NOMINMAX
#include "Mouse.h"

#include <algorithm>

using namespace Graphics;
using namespace Application;

Mouse::Mouse(HWND& hwnd) :
mLastWheelTime(0), mFrameWheel(0), mSensitivity(0.07f), mClickLimit(0.2f), mSetAbsolute(false) {
	ZeroMemory(mButtons, (size_t)MouseButton::MOUSE_MAX * sizeof(bool));
	ZeroMemory(mHoldButtons, (size_t)MouseButton::MOUSE_MAX * sizeof(bool));
	ZeroMemory(mDoubleClicks, (size_t)MouseButton::MOUSE_MAX * sizeof(bool));
	ZeroMemory(mLastClickTime, (size_t)MouseButton::MOUSE_MAX * sizeof(bool));

	mRid.usUsagePage = HID_USAGE_PAGE_GENERIC;
	mRid.usUsage = HID_USAGE_GENERIC_MOUSE;
	mRid.dwFlags = RIDEV_INPUTSINK;
	mRid.hwndTarget = hwnd;
}

Mouse::~Mouse() {
}

bool Mouse::isButtonDown(MouseButton button) {
	return mButtons[(size_t)button];
}

bool Mouse::isButtonHeld(MouseButton button) {
	return mHoldButtons[(size_t)button];
}

bool Mouse::isButtonDoubleClicked(MouseButton button) {
	return mButtons[(size_t)button] && mDoubleClicks[(size_t)button];
}

int Mouse::getMouseWheelMovement() {
	return mFrameWheel;
}

POINT Mouse::getMousePosRelative() {
	return mRelativePosition;
}

POINT Mouse::getMousePosAbsolute() {
	return mAbsolutePosition;
}

void Mouse::setDoubleClickLimit(float msec) {
	mClickLimit = msec;
}

void Mouse::setMouseSensitivity(float sensitivity) {
	mSensitivity = sensitivity == 0.0f ? 1.0f : sensitivity;
}

void Mouse::update(RAWINPUT* raw) {
	if (!mIsAwake)
		return;
	bool virtualDesktop = (raw->data.mouse.usFlags & MOUSE_VIRTUAL_DESKTOP) > 0;
	bool isAbsolute     = (raw->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE) > 0;

	if (isAbsolute) {
		const LONG screenWidth  = GetSystemMetrics(virtualDesktop ? SM_CXVIRTUALSCREEN : SM_CXSCREEN);
		const LONG screenHeight = GetSystemMetrics(virtualDesktop ? SM_CYVIRTUALSCREEN : SM_CYSCREEN);

		POINT prevAbsolute = mAbsolutePosition;
		mAbsolutePosition.x = raw->data.mouse.lLastX * screenWidth  / USHRT_MAX;
		mAbsolutePosition.y = raw->data.mouse.lLastY * screenHeight / USHRT_MAX;

		if (mSetAbsolute) {
			mRelativePosition.x = (LONG)((float)(mAbsolutePosition.x - prevAbsolute.x) * mSensitivity);
			mRelativePosition.y = (LONG)((float)(mAbsolutePosition.y - prevAbsolute.y) * mSensitivity);
		}
		mSetAbsolute = true;
	} else {
		mRelativePosition.x += (LONG)((float)raw->data.mouse.lLastX * mSensitivity);
		mRelativePosition.y += (LONG)((float)raw->data.mouse.lLastY * mSensitivity);

		mAbsolutePosition.x += raw->data.mouse.lLastX;
		mAbsolutePosition.y += raw->data.mouse.lLastY;

		mAbsolutePosition.x = std::max(std::min(mAbsolutePosition.x, mAbsolutePositionBounds.x), 0L);
		mAbsolutePosition.y = std::max(std::min(mAbsolutePosition.y, mAbsolutePositionBounds.y), 0L);
	}
	if (raw->data.mouse.usButtonFlags & RI_MOUSE_WHEEL) {
		mFrameWheel = (raw->data.mouse.usButtonData == 120) ? 1 : -1;
	}

	static int buttondowns[5] = {
		RI_MOUSE_BUTTON_1_DOWN,
		RI_MOUSE_BUTTON_2_DOWN,
		RI_MOUSE_BUTTON_3_DOWN,
		RI_MOUSE_BUTTON_4_DOWN,
		RI_MOUSE_BUTTON_5_DOWN,
	};
	static int buttonups[5] = {
		RI_MOUSE_BUTTON_1_UP,
		RI_MOUSE_BUTTON_2_UP,
		RI_MOUSE_BUTTON_3_UP,
		RI_MOUSE_BUTTON_4_UP,
		RI_MOUSE_BUTTON_5_UP,
	};

	for (int i = 0; i < 5; i++) {
		if (raw->data.mouse.usButtonFlags & buttondowns[i]) {
			mButtons[i] = true;
			if (mLastClickTime[i] > 0.0f) {
				mDoubleClicks[i] = true;
			}
			mLastClickTime[i] = mClickLimit;
		} else if (raw->data.mouse.usButtonFlags & buttonups[i]) {
			mButtons[i] = false;
			mHoldButtons[i] = false;
		}
	}
}

void Mouse::updateHolds() {
	memcpy(mHoldButtons, mButtons, (size_t)MouseButton::MOUSE_MAX * sizeof(bool));
	mRelativePosition = POINT(0, 0);
	mFrameWheel = 0;
}

void Mouse::sleep() {
	mIsAwake = false;
	mClickLimit = 0.2f;
	ZeroMemory(mHoldButtons, (size_t)MouseButton::MOUSE_MAX * sizeof(bool));
	ZeroMemory(mButtons, (size_t)MouseButton::MOUSE_MAX * sizeof(bool));
}

void Mouse::updateDoubleClick(float dt) {
	for (int i = 0; i < (int)MouseButton::MOUSE_MAX; i++) {
		if (mLastClickTime[i] <= 0.0f)
			continue;
		mLastClickTime[i] -= dt;
		if (mLastClickTime[i] > 0.0f)
			continue;
		mDoubleClicks[i] = false;
		mLastClickTime[i] = 0.0f;
	}
}

void Mouse::setAbsolutePosition(POINT pos) {
	mAbsolutePosition = pos;
}

void Mouse::setAbsolutePositionBounds(POINT bounds) {
	mAbsolutePositionBounds = bounds;
}
