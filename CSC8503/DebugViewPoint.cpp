#include "DebugViewPoint.h"
#include "Debug.h"
#include "Vector2.h"
#if WIN32
#include <Windows.h>
#include <psapi.h>
#endif

using namespace paintHell::debug;

DebugViewPoint::DebugViewPoint() {
	itemsPrinted = 0; 
	startingPoints["FrameStart"] = std::chrono::high_resolution_clock::now(); 
}

void DebugViewPoint::BeginFrame() { 
	state = enabled;
	if (state)
	{
		auto endFrame = std::chrono::high_resolution_clock::now();
		auto startFrame = startingPoints["FrameStart"];
		startingPoints["FrameStart"] = endFrame;
		if (!state) return;
		NCL::Debug::Print(std::format("{:3}FPS", (1000 / std::max(std::chrono::duration_cast<std::chrono::milliseconds>(endFrame - startFrame).count(), 1ll))), NCL::Vector2(2, 10), NCL::Debug::WHITE, 12.0f);
		itemsPrinted = 0;
		NCL::Debug::Print(std::format("memory : {}MB", (CalculateMemory() / (1024 * 1024))), NCL::Vector2(2, 12), NCL::Debug::WHITE, 12.0f);
	}
	
}

void DebugViewPoint::MarkTime(std::string name) {
	if (state)
	{
		startingPoints[name] = std::chrono::high_resolution_clock::now();
	}
}

void DebugViewPoint::FinishTime(std::string name) {
	if (state)
	{
		if (startingPoints.contains(name))
		{
			auto finishTime = std::chrono::high_resolution_clock::now();

			NCL::Debug::Print(std::format("{} : {:2}ms", name, std::chrono::duration_cast<std::chrono::milliseconds>(finishTime - startingPoints[name]).count()), NCL::Vector2(2, 14.0f + 2.0f * (++itemsPrinted)), NCL::Debug::WHITE, 12.0f);
		}
	}
}

size_t DebugViewPoint::CalculateMemory() {
#if WIN32
	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	return pmc.PrivateUsage;
#endif
	return 0;
}