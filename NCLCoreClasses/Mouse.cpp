/**
 * @file   Mouse.cpp
 * @brief  See Mouse.h.
 * 
 * @author Rich Davidson
 * @date   March 2023
 */
#include "Mouse.h"

#include <string>

using namespace NCL;

Mouse::Mouse() {
	memset(buttons      , 0, sizeof(bool)  * (int)MouseButtons::MAXVAL);
	memset(holdButtons  , 0, sizeof(bool)  * (int)MouseButtons::MAXVAL);
	memset(doubleClicks , 0, sizeof(bool)  * (int)MouseButtons::MAXVAL);
	memset(lastClickTime, 0, sizeof(float) * (int)MouseButtons::MAXVAL);

	isAwake     = true;
	lastWheel   = 0;
	frameWheel  = 0;
	sensitivity = 0.07f;
	clickLimit  = 200.0f;
}

void Mouse::UpdateFrameState(float msec) {
	memcpy(holdButtons, buttons, sizeof(bool)  * (int)MouseButtons::MAXVAL);

	relativePosition = Vector2();
	frameWheel = 0;

	for (int i = 0; i < (int)MouseButtons::MAXVAL; ++i) {
		if (lastClickTime[i] > 0) {
			lastClickTime[i] -= msec;
			if (lastClickTime[i] <= 0.0f) {
				doubleClicks[i] = false;
				lastClickTime[i] = 0.0f;
			}
		}
	}
}

void Mouse::Sleep() {
	isAwake = false; 

	memset(buttons      , 0, sizeof(bool)  * (int)MouseButtons::MAXVAL);
	memset(holdButtons  , 0, sizeof(bool)  * (int)MouseButtons::MAXVAL);
	memset(doubleClicks , 0, sizeof(bool)  * (int)MouseButtons::MAXVAL);
	memset(lastClickTime, 0, sizeof(float) * (int)MouseButtons::MAXVAL);

	lastWheel  = 0;
	frameWheel = 0;
}

void Mouse::Wake() {
	isAwake = true; 
}

void Mouse::SetAbsolutePosition(const Vector2& pos) {
	absolutePosition = pos;
}

void Mouse::SetAbsolutePositionBounds(const Vector2& bounds) {
	absolutePositionBounds = bounds;
}
