#pragma once
/**
 * @file   Main.cpp
 * @brief  
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
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

#include <chrono>
#include <thread>
#include <sstream>

using namespace NCL;
using namespace CSC8503;

//a temperory place to initial asset before we can make a asset factory class
void LoadGlobalAssets() {
	std::cout << "Loading global assets\n";
	{
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
		quad->UploadToGPU();
		AssetLibrary::AddMesh("quad", quad);
	}
	AssetLibrary::AddMesh("cube", GameTechRenderer::LoadMesh("cube.msh"));
	AssetLibrary::AddMesh("sphere", GameTechRenderer::LoadMesh("sphere.msh"));
	AssetLibrary::AddMesh("goat", GameTechRenderer::LoadMesh("Goat.msh"));
	AssetLibrary::AddMesh("capsule", GameTechRenderer::LoadMesh("capsule.msh"));

	AssetLibrary::AddMesh("fenceX", GameTechRenderer::LoadMesh("fenceXCube.msh"));
	AssetLibrary::AddMesh("fenceY", GameTechRenderer::LoadMesh("fenceYCube.msh"));
	AssetLibrary::AddMesh("wall", GameTechRenderer::LoadMesh("cube.msh"));
	AssetLibrary::AddMesh("shelter", GameTechRenderer::LoadMesh("shelterCube.msh"));
	AssetLibrary::AddMesh("pillar", GameTechRenderer::LoadMesh("pillarMsh.msh"));

	AssetLibrary::AddTexture("defaultDiffuse", GameTechRenderer::LoadTexture("DefaultDiffuse.png"));
	AssetLibrary::AddTexture("defaultBump", GameTechRenderer::LoadTexture("DefaultBump.png"));
	AssetLibrary::AddTexture("defaultSpec", GameTechRenderer::LoadTexture("DefaultSpec.png"));
	AssetLibrary::AddTexture("basic", GameTechRenderer::LoadTexture("checkerboard.png"));
	AssetLibrary::AddTexture("healingKitTex", GameTechRenderer::LoadTexture("shelterTex.jpg"));
	AssetLibrary::AddTexture("pillarTex", GameTechRenderer::LoadTexture("pillarTex.jpg"));

	AssetLibrary::AddMaterial("default", new MeshMaterial("Default.mat"));
	AssetLibrary::AddMaterial("pillar", new MeshMaterial("pillar.mat"));

	AssetLibrary::AddShader("defaultModel", GameTechRenderer::LoadShader("modelDefault.vert", "modelDefault.frag"));
	AssetLibrary::AddShader("paint", GameTechRenderer::LoadShader("modelDefault.vert", "modelPaintTexture.frag"));
}

void LoadMenuAsset() {
	std::cout << "Loading menu assets\n";
	AssetLibrary::AddTexture("menuMain", GameTechRenderer::LoadTexture("defaultMain.jpg"));
	AssetLibrary::AddTexture("menuPause", GameTechRenderer::LoadTexture("defaultpause.jpg"));
	AssetLibrary::AddTexture("menuOption", GameTechRenderer::LoadTexture("defaultMain.jpg"));

	AssetLibrary::AddTexture("button1", GameTechRenderer::LoadTexture("button1.jpg"));
	AssetLibrary::AddTexture("button2", GameTechRenderer::LoadTexture("button2.jpg"));
	AssetLibrary::AddTexture("button3", GameTechRenderer::LoadTexture("button3.jpg"));
	AssetLibrary::AddTexture("button4", GameTechRenderer::LoadTexture("button4.jpg"));
	AssetLibrary::AddTexture("button5", GameTechRenderer::LoadTexture("button5.jpg"));
	AssetLibrary::AddTexture("button6", GameTechRenderer::LoadTexture("button6.jpg"));
	AssetLibrary::AddTexture("button7", GameTechRenderer::LoadTexture("button7.jpg"));

	AssetLibrary::AddShader("menu", GameTechRenderer::LoadShader("menuVertex.vert", "menuFragment.frag"));
}

void LoadAnimationAsset() {
	std::cout << "Loading animation assets";
	AssetLibrary::AddShader("animation", GameTechRenderer::LoadShader("SkinningVertex.glsl", "TexturedFragment.glsl"));

	AssetLibrary::AddMesh("boss", GameTechRenderer::LoadMesh("Boss/Boss.msh"));

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
			std::cout << "Skipping large time delta\n";
			continue;
		}
		if (!machine.Update(dt)) {
			return;
		}
	}
}

int main() {
	Window* w = Window::CreateGameWindow("CSC8503 Game technology!", 1280, 720);

	std::cout << "loading\n";
	GameTechRenderer& renderer = GameTechRenderer::instance();
	LoadAsset();
	renderer.InitPipeline();
	
	if (!w->HasInitialised()) {
		return -1;
	}	

	StartPushdownAutomata(w);

	Window::DestroyGameWindow();
}