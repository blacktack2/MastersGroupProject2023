#include "DebugViewPoint.h"
#include "Debug.h"
#include "Vector2.h"
#include "PhysicsSystem.h"

#if WIN32
#include <Windows.h>
#include <psapi.h>
#endif

using namespace NCL;
using namespace CSC8503;
using namespace Maths;

DebugViewPoint::DebugViewPoint() {
	itemsPrinted = 0; 
	startingPoints["FrameStart"] = std::chrono::high_resolution_clock::now(); 
}

void DebugViewPoint::BeginFrame() { 
	state = enabled;
	if (state)
	{
		auto endFrame = std::chrono::high_resolution_clock::now();
		std::chrono::steady_clock::time_point startFrame = startingPoints["FrameStart"];
		startingPoints["FrameStart"] = endFrame;
		if (!state) return;
			NCL::Debug::Print(std::format("{:3}FPS", (1000 / std::max(std::chrono::duration_cast<std::chrono::milliseconds>(endFrame - startFrame).count(), 1ll))), Vector2(2, 10), NCL::Debug::WHITE, 17.0f);
			itemsPrinted = 0;
			NCL::Debug::Print(std::format("memory : {}MB", (CalculateMemory() / (1024 * 1024))), Vector2(2, 12), NCL::Debug::WHITE, 17.0f);
			NCL::Debug::Print(std::format("{:2} Total Collisions", totalCollisions), Vector2(2, 14), NCL::Debug::WHITE, 17.0f);
	}
}

void DebugViewPoint::MarkTime(std::string name) {
	if (state)
	{
		startingPoints[name] = std::chrono::high_resolution_clock::now();
	}
}

void DebugViewPoint::FinishTime(std::string name, float size) {
	if (state)
	{
		if (startingPoints.contains(name))
		{
			auto finishTime = std::chrono::high_resolution_clock::now();
			NCL::Debug::Print(std::format("{} : {:2}ms", name, std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startingPoints[name]).count() / 1000.0f), Vector2(2, 17 + 2 * (++itemsPrinted)), NCL::Debug::WHITE, size);
		}
	}
}

size_t DebugViewPoint::CalculateMemory() {
#if WIN32
	PROCESS_MEMORY_COUNTERS_EX pmc{};
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	return pmc.PrivateUsage;
#endif
	return 0;
}