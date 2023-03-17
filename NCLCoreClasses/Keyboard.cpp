/**
 * @file   Keyboard.cpp
 * @brief  See Keyboard.h.
 * 
 * @author Rich Davidson
 * @date   March 2023
 */
#include "Keyboard.h"

#include <string>

using namespace NCL;

Keyboard::Keyboard() {
	memset(keyStates , 0, (int)KeyboardKeys::MAXVALUE * sizeof(bool));
	memset(holdStates, 0, (int)KeyboardKeys::MAXVALUE * sizeof(bool));
}

void Keyboard::UpdateFrameState(float msec) {
	memcpy(holdStates, keyStates, (int)KeyboardKeys::MAXVALUE * sizeof(bool));
}

void Keyboard::Sleep() {
	isAwake = false; 
	memset(keyStates , 0, (int)KeyboardKeys::MAXVALUE * sizeof(bool));
	memset(holdStates, 0, (int)KeyboardKeys::MAXVALUE * sizeof(bool));
}

void Keyboard::Wake() {
	isAwake = true; 
}
