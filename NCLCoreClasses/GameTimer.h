/**
 * @file   GameTimer.h
 * @brief  Handler class for system time calculations.
 * 
 * @author Rich Davidson
 * @date   March 2023
 */
#pragma once
#include <chrono>

namespace NCL {
	typedef std::chrono::time_point<std::chrono::high_resolution_clock> Timepoint;

	class GameTimer {
	public:
		GameTimer();
		~GameTimer() = default;

		double GetTotalTimeSeconds() const;
		double GetTotalTimeMSec() const;

		float GetTimeDeltaSeconds() const { return timeDelta; };
		float GetTimeDeltaMSec() const { return timeDelta * 1000.0f; };

		void Tick();
	protected:
		float timeDelta;
		Timepoint firstPoint;
		Timepoint nowPoint;
	};
}
