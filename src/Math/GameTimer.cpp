/**
 * @file   GameTimer.cpp
 * @brief  See GameTimer.h.
 * 
 * @author Stuart Lewis
 * @date   January 2023
 */
#include "GameTimer.h"

using namespace Math;

GameTimer::GameTimer() :
mLastFrame(), mCurrentFrame(std::chrono::steady_clock::now()) {

}

GameTimer::~GameTimer() {
}

void GameTimer::updateTime() {
	mLastFrame = mCurrentFrame;
	mCurrentFrame = std::chrono::steady_clock::now();
}
