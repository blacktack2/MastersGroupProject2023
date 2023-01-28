/**
 * @file   GameTimer.h
 * @brief  Handler class for dealing with frame time calculations.
 * 
 * @author Stuart Lewis
 * @date   January 2023
 */
#pragma once
#include <chrono>

namespace Math {
	typedef std::chrono::duration<float, std::nano> time_delta_t;

	/**
	 * @brief Handler class for dealing with frame time calculations.
	 */
	class GameTimer {
	public:
		/**
		 * @brief Re-calculate the time since last frame. Should be called
		 * once every frame.
		 */
		void updateTime();

		/**
		 * @returns Time since last frame. See std::chrono::duration.
		 */
		inline time_delta_t getDT() {
			return mCurrentFrame - mLastFrame;
		}
		/**
		 * @returns Time (milliseconds) since last frame.
		 */
		inline float getMS() {
			return std::chrono::duration_cast<std::chrono::milliseconds>(mCurrentFrame - mLastFrame).count();
		}

		/**
		 * @brief Singleton getter for the GameTimer.
		 */
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
