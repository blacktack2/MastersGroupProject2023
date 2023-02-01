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
#include "BehaviourAction.h"

using namespace NCL;
using namespace CSC8503;

#include <chrono>
#include <thread>
#include <sstream>

void TestStateMachine() {
	StateMachine* testMachine = new StateMachine();
	int data = 0;

	State* A = new State([&](float dt)->void {
		std::cout << "In state A\n";
		data++;
	});
	State* B = new State([&](float dt)->void {
		std::cout << "In state B\n";
		data--;
	});

	StateTransition* stateAB = new StateTransition(A, B, [&](void)->bool {
		return data > 10;
	});
	StateTransition* stateBA = new StateTransition(B, A, [&](void)->bool {
		return data < 0;
	});

	testMachine->AddState(A);
	testMachine->AddState(B);
	testMachine->AddTransition(stateAB);
	testMachine->AddTransition(stateBA);

	for (int i = 0; i < 100; i++) {
		testMachine->Update(1.0f);
	}
}

void TestBehaviourTree() {
	float behaviourTimer;
	float distanceToTarget;

	BehaviourAction* findKey = new BehaviourAction("Find Key", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			std::cout << "Looking for key!\n";
			behaviourTimer = rand() % 100;
			return Ongoing;
		} else if (state == Ongoing) {
			behaviourTimer -= dt;
			if (behaviourTimer <= 0.0f) {
				std::cout << "Found a key!\n";
				return Success;
			}
		}
		return state;
	});

	BehaviourAction* goToRoom = new BehaviourAction("Go To Room", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			std::cout << "Going to the loot room!\n";
			return Ongoing;
		} else if (state == Ongoing) {
			distanceToTarget -= dt;
			if (distanceToTarget <= 0.0f) {
				std::cout << "Reached room!\n";
				return Success;
			}
		}
		return state;
	});

	BehaviourAction* openDoor = new BehaviourAction("Open Door", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			std::cout << "Opening Door!\n";
			return Success;
		}
		return state;
	});

	BehaviourAction* lookForTreasure = new BehaviourAction("Look For Treasure", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			std::cout << "Looking for treasure!\n";
			return Ongoing;
		} else if (state == Ongoing) {
			bool found = rand() % 2;
			if (found) {
				std::cout << "Treasure found!\n";
				return Success;
			}
			std::cout << "No treasure...\n";
			return Failure;
		}
		return state;
	});

	BehaviourAction* lookForItems = new BehaviourAction("Look For Items", [&](float dt, BehaviourState state)->BehaviourState {
		if (state == Initialise) {
			std::cout << "Looking for items!\n";
			return Ongoing;
		} else if (state == Ongoing) {
			bool found = rand() % 2;
			if (found) {
				std::cout << "Items found!\n";
				return Success;
			}
			std::cout << "No items...\n";
			return Failure;
		}
		return state;
	});

	BehaviourSequence* roomSequence = new BehaviourSequence("Room Sequence");
	roomSequence->AddChild(findKey);
	roomSequence->AddChild(goToRoom);
	roomSequence->AddChild(openDoor);

	BehaviourParallel* lootSelection = new BehaviourParallel("Loot Selection");
	lootSelection->AddChild(lookForTreasure);
	lootSelection->AddChild(lookForItems);

	BehaviourSequence* behaviourRoot = new BehaviourSequence("Root Sequence");
	behaviourRoot->AddChild(roomSequence);
	behaviourRoot->AddChild(lootSelection);

	std::cout << "Beginning Behaviour Tree test\n";
	for (int i = 0; i < 5; i++) {
		behaviourRoot->Reset();
		behaviourTimer = 0.0f;
		distanceToTarget = rand() % 250;
		BehaviourState state = Ongoing;
		std::cout << "Going on an adventure!\n";
		while (state == Ongoing) {
			state = behaviourRoot->Execute(1.0f);
		}
		if (state == Success) {
			std::cout << "A successful excursion!\n";
		} else if (state == Failure) {
			std::cout << "A waste of time!\n";
		}
	}
	std::cout << "Behaviour Tree test complete\n";
}

static std::vector<Vector3> testNodes;

void TestPathfinding() {
	NavigationGrid grid("TestGrid1.txt");

	NavigationPath outPath;

	Vector3 startPos(70, 0, 10);
	Vector3 endPos(60, 0, 80);

	bool found = grid.FindPath(startPos, endPos, outPath);

	Vector3 pos;
	while (outPath.PopWaypoint(pos)) {
		testNodes.push_back(pos);
	}
}

void DisplayPathfinding() {
	for (int i = 1; i < testNodes.size(); i++) {
		Vector3 a = testNodes[i - 1];
		Vector3 b = testNodes[i];

		Debug::DrawLine(a, b, Vector4(0, 1, 0, 1));
	}
}

class PauseScreen : public PushdownState {
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::U)) {
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;
	}
	void OnAwake() override {
		std::cout << "Press U to unpause game\n";
	}
};

class GameScreen : public PushdownState {
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		pauseReminder -= dt;
		if (pauseReminder < 0) {
			std::cout << "Coins mined: " << coinsMined << "\n";
			std::cout << "Press P to pause the game,\n or C to return to main menu\n";
			pauseReminder += 1.0f;
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::P)) {
			*newState = new PauseScreen();
			return PushdownResult::Push;
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::C)) {
			std::cout << "Returning to main menu!\n";
			return PushdownResult::Pop;
		}
		if (rand() % 7 == 0) {
			coinsMined++;
		}
		return PushdownResult::NoChange;
	}
	void OnAwake() override {
		std::cout << "Preparing to mine coins!\n";
	}
protected:
	int coinsMined = 0;
	float pauseReminder = 1;
};

class IntroScreen : public PushdownState {
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE)) {
			*newState = new GameScreen();
			return PushdownResult::Push;
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)) {
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;
	}
	void OnAwake() override {
		std::cout << "Welcome!\n";
		std::cout << "Press space to begin, or escape to quit!\n";
	}
};

void TestPushdownAutomata(Window* w) {
	PushdownMachine machine(new IntroScreen());
	while (w->UpdateWindow()) {
		float dt = w->GetTimer()->GetTimeDeltaSeconds();
		if (!machine.Update(dt)) {
			return;
		}
	}
}

class TestPacketReceiver : public PacketReceiver {
public:
	TestPacketReceiver(std::string name) {
		this->name = name;
	}

	void ReceivePacket(int type, GamePacket* payload, int source) {
		if (type == String_Message) {
			StringPacket* realPacket = (StringPacket*)payload;

			std::string msg = realPacket->GetStringFromData();

			std::cout << name << " received message: " << msg << "\n";
		}
	}
protected:
	std::string name;
};

void TestNetworking() {
	NetworkBase::Initialise();

	TestPacketReceiver serverReceiver("Server");
	TestPacketReceiver clientReceivers[2]{ TestPacketReceiver("Client1"), TestPacketReceiver("Client2") };

	int port = NetworkBase::GetDefaultPort();

	const int numClients = 2;
	GameServer* server = new GameServer(port, numClients);
	GameClient* clients = new GameClient[numClients];

	server->RegisterPacketHandler(String_Message, &serverReceiver);
	for (int i = 0; i < numClients; i++) {
		clients[i].RegisterPacketHandler(String_Message, &clientReceivers[i]);
	}

	bool canConnect[numClients];
	for (int i = 0; i < numClients; i++) {
		canConnect[i] = clients[i].Connect(127, 0, 0, 1, port);
	}

	for (int i = 0; i < 100; i++) {
		server->SendGlobalPacket((GamePacket&)StringPacket("Server says hello!" + std::to_string(i)));
		for (int j = 0; j < numClients; j++) {
			clients[j].SendPacket((GamePacket&)StringPacket("Client says hello!" + std::to_string(i)));
		}

		server->UpdateServer();
		for (int j = 0; j < numClients; j++) {
			clients[j].UpdateClient();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	NetworkBase::Destroy();
}

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
	//TestNetworking();

	Window* w = Window::CreateGameWindow("CSC8503 Game technology!", 1280, 720);
	//TestPushdownAutomata(w);

	if (!w->HasInitialised()) {
		return -1;
	}	

	w->ShowOSPointer(false);
	w->LockMouseToWindow(true);

	TutorialGame* g = new TutorialGame();
	g->InitWorld();
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
	Window::DestroyGameWindow();
}