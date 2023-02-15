#pragma once
#include "Window.h"

#include "Debug.h"

#include "StateMachine.h"
#include "StateTransition.h"
#include "State.h"

#include "GameServer.h"
#include "GameClient.h"

#include "NavigationGrid.h"
#include "NavigationMesh.h"

#include "TutorialGame.h"
#include "NetworkedGame.h"

#include "PushdownMachine.h"

#include "PushdownState.h"

#include "BehaviourNode.h"
#include "BehaviourSelector.h"
#include "BehaviourSequence.h"
#include "BehaviourParallel.h"
#include "BehaviourParallelSelector.h"
#include "BehaviourAction.h"

#include "TestAudio.h"

using namespace NCL;
using namespace CSC8503;

#include <chrono>
#include <thread>
#include <sstream>


/*

The main function should look pretty familar to you!
We make a window, and then go into a while loop that repeatedly
runs our 'game' until we press escape. Instead of making a 'renderer'
and updating it, we instead make a whole game, and repeatedly update that,
instead. 

This time, we've added some extra functionality to the window class - we can
hide or show the 

*/
int main() {
	//TestStateMachine();
	//TestBehaviourTree();
	//TestPathfinding();

	Window* w = Window::CreateGameWindow("CSC8503 Game technology!", 1280, 720);
	//TestPushdownAutomata(w);

	
	if (!w->HasInitialised()) {
		return -1;
	}	

	//TestNetworking();

	w->ShowOSPointer(false);
	w->LockMouseToWindow(true);
	//TestAudio::TestAudio2();

	NetworkedGame* g = new NetworkedGame();
	g->InitWorld(NCL::CSC8503::TutorialGame::InitMode::AUDIO_TEST);
	w->GetTimer()->GetTimeDeltaSeconds(); //Clear the timer so we don't get a larget first dt!
	while (w->UpdateWindow() && !g->IsQuit()) {
		float dt = w->GetTimer()->GetTimeDeltaSeconds();
		if (dt > 0.1f) {
			std::cout << "Skipping large time delta" << std::endl;
			continue; //must have hit a breakpoint or something to have a 1 second frame time!
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::PRIOR)) {
			w->ShowConsole(true);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NEXT)) {
			w->ShowConsole(false);
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::T)) {
			w->SetWindowPosition(0, 0);
		}

		w->SetTitle("Gametech frame time:" + std::to_string(1000.0f * dt));

		g->UpdateGame(dt);
		//DisplayPathfinding();
	}
	delete g;
	Window::DestroyGameWindow();
}