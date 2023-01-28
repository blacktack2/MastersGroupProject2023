#pragma once
#include <chrono>

namespace Math {
	typedef std::chrono::duration<float, std::nano> time_delta_t;

	class GameTimer {
	public:
		void updateTime();

		inline time_delta_t getDT() {
			return mCurrentFrame - mLastFrame;
		}

		inline float getMS() {
			return std::chrono::duration_cast<std::chrono::milliseconds>(mCurrentFrame - mLastFrame).count();
		}

		static inline GameTimer& getTimer() {
			static GameTimer gameTimer = GameTimer();
			return gameTimer;
		}
	private:
		GameTimer();
		~GameTimer();

		std::chrono::time_point<std::chrono::steady_clock> mLastFrame;
		std::chrono::time_point<std::chrono::steady_clock> mCurrentFrame;
	};
}
