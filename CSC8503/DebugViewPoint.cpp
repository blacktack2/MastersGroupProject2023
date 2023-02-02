#include "DebugViewPoint.h"
#include "Debug.h"
#include "Vector2.h"

using namespace std;
using namespace paintHell::debug;

void DebugViewPoint::MarkTime(std::string name) {
	startingPoints[name] = chrono::high_resolution_clock::now();
}

void DebugViewPoint::FinishTime(string name) {
	auto finishTime = chrono::high_resolution_clock::now();

	NCL::Debug::Print(std::format("{} : {:2}ms", name, chrono::duration_cast<chrono::milliseconds>(finishTime - startingPoints[name]).count()), NCL::Vector2(2, 10+2*(++itemsPrinted)), NCL::Debug::WHITE, 12.0f);
}