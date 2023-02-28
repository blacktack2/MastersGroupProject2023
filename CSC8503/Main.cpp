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

#include "TestAudio.h"

#include "ScreenMain.h"

#include "AssetLibrary.h"

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

//a temperory place to initial asset before we can make a asset factory class
void LoadGlobalAssets() {
	GameTechRenderer* renderer = &GameTechRenderer::instance();

	//basic assets
	AssetLibrary::AddMesh("cube", renderer -> LoadMesh("cube.msh"));
	AssetLibrary::AddMesh("sphere", renderer -> LoadMesh("sphere.msh"));
	AssetLibrary::AddMesh("goat", renderer -> LoadMesh("Goat.msh"));
	AssetLibrary::AddMesh("capsule", renderer -> LoadMesh("capsule.msh"));

	AssetLibrary::AddMesh("fenceX", renderer -> LoadMesh("fenceXCube.msh"));
	AssetLibrary::AddMesh("fenceY", renderer -> LoadMesh("fenceYCube.msh"));
	AssetLibrary::AddMesh("wall", renderer -> LoadMesh("cube.msh"));
	AssetLibrary::AddMesh("shelter", renderer -> LoadMesh("shelterCube.msh"));
	AssetLibrary::AddMesh("pillar", renderer->LoadMesh("pillarMsh.msh"));

	AssetLibrary::AddTexture("basic", renderer->LoadTexture("checkerboard.png"));
	AssetLibrary::AddTexture("healingKitTex", renderer->LoadTexture("shelterTex.jpg"));
	AssetLibrary::AddTexture("pillarTex", renderer->LoadTexture("pillarTex.jpg"));

	AssetLibrary::AddShader("paint", renderer->LoadShader("modelDefault.vert", "modelPaintTexture.frag"));

}

void LoadMenuAsset() {
	GameTechRenderer* renderer = &GameTechRenderer::instance();
	OGLMesh* quad = new OGLMesh();
	quad->SetVertexPositions({
		Vector3(-1, 1, -1),
		Vector3(-1, -1, -1),
		Vector3(1, -1, -1),
		Vector3(1, 1, -1),
		});
	quad->SetVertexTextureCoords({
		Vector2(0, 0),
		Vector2(0, 1),
		Vector2(1, 1),
		Vector2(1, 0),
		});
	quad->SetVertexIndices({ 0, 1, 2, 2, 3, 0 });
	quad->SetVertexColours({
		Vector4(1, 1, 1, 1),
		Vector4(1, 1, 1, 1),
		Vector4(1, 1, 1, 1),
		Vector4(1, 1, 1, 1),
		});
	AssetLibrary::AddMesh("quad", quad);

	AssetLibrary::AddTexture("menuMain", renderer->LoadTexture("defaultMain.jpg"));
	AssetLibrary::AddTexture("menuPause", renderer->LoadTexture("defaultpause.jpg"));
	AssetLibrary::AddTexture("menuOption", renderer->LoadTexture("defaultMain.jpg"));

	AssetLibrary::AddTexture("button1", renderer->LoadTexture("button1.jpg"));
	AssetLibrary::AddTexture("button2", renderer->LoadTexture("button2.jpg"));
	AssetLibrary::AddTexture("button3", renderer->LoadTexture("button3.jpg"));
	AssetLibrary::AddTexture("button4", renderer->LoadTexture("button4.jpg"));
	AssetLibrary::AddTexture("button5", renderer->LoadTexture("button5.jpg"));
	AssetLibrary::AddTexture("button6", renderer->LoadTexture("button6.jpg"));
	AssetLibrary::AddTexture("button7", renderer->LoadTexture("button7.jpg"));

	AssetLibrary::AddShader("menu", renderer->LoadShader("menuVertex.vert", "menuFragment.frag"));
}

void LoadAnimationAsset() {
	GameTechRenderer* renderer = &GameTechRenderer::instance();
	AssetLibrary::AddShader("animation", renderer->LoadShader("SkinningVertex.glsl", "TexturedFragment.glsl"));

	AssetLibrary::AddMesh("boss", renderer->LoadMesh("Boss/Boss.msh"));

	AssetLibrary::AddMaterial("boss", new MeshMaterial("Boss/Boss.mat"));

	AssetLibrary::AddAnimation("WalkForward", new MeshAnimation("Boss/walk.anm"));
	AssetLibrary::AddAnimation("Jump",	  new MeshAnimation("Boss/Jump.anm"));
	AssetLibrary::AddAnimation("Attack1", new MeshAnimation("Boss/SillyDancing.anm"));
	AssetLibrary::AddAnimation("Attack2", new MeshAnimation("Boss/HipHopDancing.anm"));
	AssetLibrary::AddAnimation("Attack3", new MeshAnimation("Boss/JoyfulJump.anm"));
	AssetLibrary::AddAnimation("Attack4", new MeshAnimation("Boss/RumbaDancing.anm"));
	AssetLibrary::AddAnimation("Attack5", new MeshAnimation("Boss/NorthernSoulSpin.anm"));
	AssetLibrary::AddAnimation("Attack6", new MeshAnimation("Boss/SambaDancing.anm"));
}

void LoadAsset() {
	LoadMenuAsset();
	LoadGlobalAssets();
	LoadAnimationAsset();
}


void StartPushdownAutomata(Window* w) {
	PushdownMachine machine(new ScreenMain());
	while (w->UpdateWindow()) {
		float dt = w->GetTimer()->GetTimeDeltaSeconds();
		if (dt > 0.1f) {
			std::cout << "Skipping large time delta" << std::endl;
			continue; //must have hit a breakpoint or something to have a 1 second frame time!
		}
		if (!machine.Update(dt)) {
			return;
		}
	}
}

int main() {

	Window* w = Window::CreateGameWindow("CSC8503 Game technology!", 1280, 720);

	std::cout << "loading" << std::endl;
	LoadAsset();
	
	if (!w->HasInitialised()) {
		return -1;
	}	

	StartPushdownAutomata(w);


	/*
	w->ShowOSPointer(false);
	w->LockMouseToWindow(true);

	NetworkedGame* g = new NetworkedGame();
	//g->InitWorld(NCL::CSC8503::TutorialGame::InitMode::AUDIO_TEST);
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
	*/
	Window::DestroyGameWindow();
}