#include "DebugViewPoint.h"
#include "Debug.h"
#include "Vector2.h"
#if WIN32
#include <Windows.h>
#include <psapi.h>
#endif

using namespace std;
using namespace paintHell::debug;

DebugViewPoint::DebugViewPoint() { 
	enabled = false; 
	itemsPrinted = 0; 
	startingPoints["FrameStart"] = chrono::high_resolution_clock::now(); 
}

void DebugViewPoint::BeginFrame() { 
	auto endFrame = chrono::high_resolution_clock::now();
	auto startFrame = startingPoints["FrameStart"];
	startingPoints.clear();
	startingPoints["FrameStart"] = endFrame;
	NCL::Debug::Print(std::format("{:3}FPS", (1000 / max(chrono::duration_cast<chrono::milliseconds>(endFrame - startFrame).count(),1))), NCL::Vector2(2, 10), NCL::Debug::WHITE, 12.0f);
	itemsPrinted = 0;
	NCL::Debug::Print(std::format("memory : {}MB", (CalculateMemory() / (1024 * 1024))), NCL::Vector2(2, 12), NCL::Debug::WHITE, 12.0f);
}

void DebugViewPoint::MarkTime(std::string name) {
	startingPoints[name] = chrono::high_resolution_clock::now();
}

void DebugViewPoint::FinishTime(string name) {
	auto finishTime = chrono::high_resolution_clock::now();

	NCL::Debug::Print(std::format("{} : {:2}ms", name, chrono::duration_cast<chrono::milliseconds>(finishTime - startingPoints[name]).count()), NCL::Vector2(2, 14+2*(++itemsPrinted)), NCL::Debug::WHITE, 12.0f);
}

size_t DebugViewPoint::CalculateMemory() {
#if WIN32
	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	return pmc.PrivateUsage;
#endif
	return 0;
}