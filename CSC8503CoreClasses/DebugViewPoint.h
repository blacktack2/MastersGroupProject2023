#pragma once
#include <chrono>
#include <string>



namespace NCL {
	namespace CSC8503 {
		class OptionManager;
	}

	using namespace CSC8503;

	class DebugViewPoint
	{
	public:
		bool enabled = false;

		static DebugViewPoint& Instance() {
			static DebugViewPoint INSTANCE;
			return INSTANCE;
		}
		
		/**
		* @breif called at the start of the main loop to mark the begining of a frame
		* @breif it will also display the memory used and current FPS
		*/
		void BeginFrame();

		/**
		* @breif used to mark the start of a timer
		* @param name the display name of the timer
		*/
		void MarkTime(std::string name);

		/**
		* @breif used to finish and display a timmer
		* @param name the name of the timer to finish
		*/
		void FinishTime(std::string name, float size = 15.0f);

		void SetCollisions(int value) { totalCollisions = value; }
		
	private:
		DebugViewPoint();
		~DebugViewPoint() {}
		size_t CalculateMemory();

		bool state = true;
		std::map<std::string, std::chrono::steady_clock::time_point> startingPoints;
		int itemsPrinted;
		int totalCollisions = 0;
	};
}

