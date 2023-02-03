#pragma once
#include <chrono>
#include <string>

namespace paintHell::debug {
	class DebugViewPoint
	{
	public:
		bool enabled;

		static DebugViewPoint& Instance() {
			static DebugViewPoint INSTANCE;
			return INSTANCE;
		}
		

		void BeginFrame();
		void MarkTime(std::string name);
		void FinishTime(std::string name);

		

	private:
		size_t CalculateMemory();

		std::map<std::string, std::chrono::steady_clock::time_point> startingPoints;
		int itemsPrinted;
		DebugViewPoint();
		~DebugViewPoint() {}
	};
}

