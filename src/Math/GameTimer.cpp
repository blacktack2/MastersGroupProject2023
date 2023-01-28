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
