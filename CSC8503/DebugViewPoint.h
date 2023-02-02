#pragma once
#include <chrono>
#include <string>

namespace paintHell::debug {
	class DebugViewPoint
	{
	public:
		bool enabled;

		DebugViewPoint() { enabled = false; itemsPrinted = 0; }
		~DebugViewPoint() {}

		void BeginFrame() { startingPoints.clear(); itemsPrinted = 0; }
		void MarkTime(std::string name);
		void FinishTime(std::string name);

	private:
		std::map<std::string, std::chrono::steady_clock::time_point> startingPoints;
		int itemsPrinted;
	};
}

