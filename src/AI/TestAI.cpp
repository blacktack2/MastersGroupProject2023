/**
 * @file   TestAI.cpp
 * @brief  Test AI operations
 *
 * @author Daniel Thomas Abraham
 * @date   January 2023
 */

#include "StateMachine.h"
#include "StateTransition.h"
#include "State.h"

#include "BehaviourNode.h"
#include "BehaviourSelector.h"
#include "BehaviourSequence.h"
#include "BehaviourAction.h"
#include "BehaviourParallel.h"

#include "TestAI.h"

#include<iostream>

using namespace paintHell::core;

void TestAI::TestStateMachine() {
	StateMachine* testMachine = new StateMachine();
	int data = 0;

	State* A = new State([&](float dt)->void {
		std::cout << "I'm in State A!\n";
		data++;
		}
	);

	State* B = new State([&](float dt)->void {
		std::cout << "I'm in state B\n";
		data--;
		}
	);

	StateTransition* stateAB = new StateTransition(A, B, [&](const StateTransition& s)->bool {
		return data > 10;
		}
	);

	StateTransition* stateBA = new StateTransition(B, A, [&](const StateTransition& s)->bool {
		return data < 0;
		}
	);

	testMachine->AddState(A);
	testMachine->AddState(B);
	testMachine->AddTransition(stateAB);
	testMachine->AddTransition(stateBA);

	for (int i = 0; i < 100; i++) {
		testMachine->Update(1.0f);
	}

}
void TestAI::TestBehaviourTree() {
	float behaviourTimer;
	float distanceToTarget;
	BehaviourAction* smile = new BehaviourAction("Smile", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			std::cout << "Smile!"<<std::endl;
			behaviourTimer = rand() % 100;
			state = Ongoing;
		}
		else if (state == Ongoing) {
			behaviourTimer -= dt;
			if (behaviourTimer <= 0.0f) {
				std::cout << "Be serious!"<<std::endl;
				return Success;
			}
		}
		return state;
		});

	BehaviourAction* findKey = new BehaviourAction("FindKey", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			std::cout << "Looking for the key\n";
			behaviourTimer = rand() % 100;
			state = Ongoing;
		}
		else if (state == Ongoing) {
			behaviourTimer -= dt;
			if (behaviourTimer <= 0.0f) {
				std::cout << "Found the key!\n";
				return Success;
			}
		}
		return state;
		});
	BehaviourAction* goToRoom = new BehaviourAction("GoToRoom", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			std::cout << "Going to Loot room\n";
			state = Ongoing;
		}
		else if (state == Ongoing) {
			distanceToTarget -= dt;
			if (distanceToTarget <= 0.0f) {
				std::cout << "Reached the room!\n";
				return Success;
			}
		}
		return state;
		});
	BehaviourAction* openDoor = new BehaviourAction("OpenDoor", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			std::cout << "Opening door\n";
			state = Success;
		}
		return state;
		});
	BehaviourAction* findTreasure = new BehaviourAction("Look for Treasure", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			std::cout << "Looking for treasure\n";
			state = Ongoing;
		}
		else if (state == Ongoing) {
			bool found = rand() % 2;
			if (found) {
				std::cout << "I have found some treasure!\n";
				return Success;
			}
			std::cout << "No treasure here\n";
			return Failure;
		}
		return state;
		});
	BehaviourAction* findItems = new BehaviourAction("Look for Items", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			std::cout << "Looking for items\n";
			state = Ongoing;
		}
		else if (state == Ongoing) {
			bool found = rand() % 2;
			if (found) {
				std::cout << "I have found some items!\n";
				return Success;
			}
			std::cout << "No items in here\n";
			return Failure;
		}
		return state;
		});

	BehaviourSequence* sequence = new BehaviourSequence("Room sequence");
	sequence->AddChild(findKey);
	sequence->AddChild(goToRoom);
	sequence->AddChild(openDoor);

	BehaviourSelector* selection = new BehaviourSelector("Loot Selection");
	selection->AddChild(findTreasure);
	selection->AddChild(findItems);

	BehaviourSequence* rootSequence = new BehaviourSequence("Root Sequence");
	rootSequence->AddChild(sequence);
	rootSequence->AddChild(selection);


	BehaviourParallel* rootParallel = new BehaviourParallel("Main parallel action");
	rootParallel->AddChild(rootSequence);
	rootParallel->AddChild(smile);

	for (int i = 0; i < 5; i++)
	{
		std::cout << "------\nA new adventure\n-----\n";
		rootParallel->Reset();
		behaviourTimer = 0.0f;
		distanceToTarget = rand() % 251;
		BehaviourState state = Ongoing;
		while (state == Ongoing) {
			state = rootParallel->Execute(1.0f);
		}
		if (state == Success) {
			std::cout << "What a successful adventure\n";
			
		}
		else if (state == Failure) {
			std::cout << "What a waste of time\n";
		}
	}
	std::cout << "All done!\n";
}