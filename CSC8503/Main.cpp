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

#include "PlayerBullet.h"

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
#include "AssetLoader.h"
#include "PrefabLibrary.h"

#include "MeshAnimation.h"
#include "MeshGeometry.h"
#include "MeshMaterial.h"
#include "ShaderBase.h"
#include "TextureBase.h"

#include <chrono>
#include <thread>
#include <sstream>

using namespace NCL;
using namespace CSC8503;

// TODO Move asset loading functions to a specialised class
void LoadGlobalAssets() {
	std::cout << "Loading global assets\n";
	{
		std::shared_ptr<MeshGeometry> quad = AssetLoader::CreateMesh();
		quad->SetVertexPositions({
			Vector3(-1, 1, -1),
			Vector3(-1, -1, -1),
			Vector3(1, -1, -1),
			Vector3(1, 1, -1),
			});
		quad->SetVertexTextureCoords({
			Vector2(0, 1),
			Vector2(0, 0),
			Vector2(1, 0),
			Vector2(1, 1),
			});
		quad->SetVertexIndices({ 0, 1, 2, 2, 3, 0 });
		quad->SetVertexColours({
			Vector4(1, 1, 1, 1),
			Vector4(1, 1, 1, 1),
			Vector4(1, 1, 1, 1),
			Vector4(1, 1, 1, 1),
			});
		quad->UploadToGPU();
		AssetLibrary<MeshGeometry>::AddAsset("quad", std::move(quad));
	}
	AssetLibrary<MeshGeometry>::AddAsset("cube", AssetLoader::LoadMesh("cube.msh"));
	AssetLibrary<MeshGeometry>::AddAsset("sphere", AssetLoader::LoadMesh("sphere.msh"));
	AssetLibrary<MeshGeometry>::AddAsset("goat", AssetLoader::LoadMesh("Goat.msh"));
	AssetLibrary<MeshGeometry>::AddAsset("capsule", AssetLoader::LoadMesh("capsule.msh"));

	AssetLibrary<MeshGeometry>::AddAsset("fenceX", AssetLoader::LoadMesh("fenceXCube.msh"));
	AssetLibrary<MeshGeometry>::AddAsset("fenceY", AssetLoader::LoadMesh("fenceYCube.msh"));
	AssetLibrary<MeshGeometry>::AddAsset("wall", AssetLoader::LoadMesh("cube.msh"));
	AssetLibrary<MeshGeometry>::AddAsset("shelter", AssetLoader::LoadMesh("shelterCube.msh"));
	AssetLibrary<MeshGeometry>::AddAsset("pillar", AssetLoader::LoadMesh("pillarMsh.msh"));

	AssetLibrary<TextureBase>::AddAsset("defaultAlbedo", AssetLoader::LoadTexture("DefaultAlbedo.png"));
	AssetLibrary<TextureBase>::AddAsset("defaultBump", AssetLoader::LoadTexture("DefaultBump.png"));
	AssetLibrary<TextureBase>::AddAsset("defaultSpec", AssetLoader::LoadTexture("DefaultSpec.png"));
	AssetLibrary<TextureBase>::AddAsset("basic", AssetLoader::LoadTexture("checkerboard.png"));
	AssetLibrary<TextureBase>::AddAsset("healingKitTex", AssetLoader::LoadTexture("shelterTex.jpg"));
	AssetLibrary<TextureBase>::AddAsset("pillarTex", AssetLoader::LoadTexture("pillarTex.jpg"));

	AssetLibrary<MeshMaterial>::AddAsset("default", std::make_shared<MeshMaterial>("Default.mat"));
	AssetLibrary<MeshMaterial>::AddAsset("pillar", std::make_shared<MeshMaterial>("pillar.mat"));

	AssetLibrary<ShaderBase>::AddAsset("modelDefault", std::move(AssetLoader::CreateShader("modelDefault.vert", "modelDefault.frag")));
	AssetLibrary<ShaderBase>::AddAsset("paintDefault", std::move(AssetLoader::CreateShader("modelDefault.vert", "modelPaintTexture.frag")));
	AssetLibrary<ShaderBase>::AddAsset("animationDefault", std::move(AssetLoader::CreateShader("modelAnimated.vert", "modelAnimated.frag")));
}

void LoadMenuAsset() {
	std::cout << "Loading menu assets\n";
	AssetLibrary<TextureBase>::AddAsset("menuMain", AssetLoader::LoadTexture("defaultMain.jpg"));
	AssetLibrary<TextureBase>::AddAsset("menuPause", AssetLoader::LoadTexture("defaultpause.jpg"));
	AssetLibrary<TextureBase>::AddAsset("menuOption", AssetLoader::LoadTexture("defaultMain.jpg"));

	AssetLibrary<TextureBase>::AddAsset("button1", AssetLoader::LoadTexture("button1.jpg"));
	AssetLibrary<TextureBase>::AddAsset("button2", AssetLoader::LoadTexture("button2.jpg"));
	AssetLibrary<TextureBase>::AddAsset("button3", AssetLoader::LoadTexture("button3.jpg"));
	AssetLibrary<TextureBase>::AddAsset("button4", AssetLoader::LoadTexture("button4.jpg"));
	AssetLibrary<TextureBase>::AddAsset("button5", AssetLoader::LoadTexture("button5.jpg"));
	AssetLibrary<TextureBase>::AddAsset("button6", AssetLoader::LoadTexture("button6.jpg"));
	AssetLibrary<TextureBase>::AddAsset("button7", AssetLoader::LoadTexture("button7.jpg"));

	AssetLibrary<TextureBase>::AddAsset("fontAtlas", AssetLoader::LoadTexture("PressStart2P.png"));

	AssetLibrary<ShaderBase>::AddAsset("menu", std::move(AssetLoader::CreateShader("menuVertex.vert", "menuFragment.frag")));
}

void LoadAnimationAsset() {
	std::cout << "Loading animation assets";
	AssetLibrary<MeshGeometry>::AddAsset("boss", AssetLoader::LoadMesh("Boss/Boss.msh"));

	AssetLibrary<MeshMaterial>::AddAsset("boss", std::make_shared<MeshMaterial>("Boss/Boss.mat"));

	AssetLibrary<MeshAnimation>::AddAsset("WalkForward", std::make_shared<MeshAnimation>("Boss/walk.anm"));
	AssetLibrary<MeshAnimation>::AddAsset("Jump", std::make_shared<MeshAnimation>("Boss/Jump.anm"));
	AssetLibrary<MeshAnimation>::AddAsset("Attack1", std::make_shared<MeshAnimation>("Boss/SillyDancing.anm"));
	AssetLibrary<MeshAnimation>::AddAsset("Attack2", std::make_shared<MeshAnimation>("Boss/HipHopDancing.anm"));
	AssetLibrary<MeshAnimation>::AddAsset("Attack3", std::make_shared<MeshAnimation>("Boss/JoyfulJump.anm"));
	AssetLibrary<MeshAnimation>::AddAsset("Attack4", std::make_shared<MeshAnimation>("Boss/RumbaDancing.anm"));
	AssetLibrary<MeshAnimation>::AddAsset("Attack5", std::make_shared<MeshAnimation>("Boss/NorthernSoulSpin.anm"));
	AssetLibrary<MeshAnimation>::AddAsset("Attack6", std::make_shared<MeshAnimation>("Boss/SambaDancing.anm"));
}

void LoadPrefabs() {
	float bulletRadius = 0.2f;
	std::unique_ptr<GameObject> bulletPrefab = std::make_unique<PlayerBullet>();

	bulletPrefab->SetBoundingVolume((CollisionVolume*) new SphereVolume(bulletRadius, CollisionLayer::PlayerProj));
	bulletPrefab->GetTransform().SetScale(Vector3(bulletRadius));

	bulletPrefab->SetRenderObject(new RenderObject(bulletPrefab->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("sphere"), nullptr));
	bulletPrefab->SetPhysicsObject(new PhysicsObject(&bulletPrefab->GetTransform(), bulletPrefab->GetBoundingVolume(), true));
	bulletPrefab->GetRenderObject()->SetColour(Vector4(1, 0.5f, 0.8f, 1.0f));

	bulletPrefab->GetPhysicsObject()->SetInverseMass(1.0f);
	bulletPrefab->GetPhysicsObject()->SetGravWeight(1.0f);
	bulletPrefab->GetPhysicsObject()->InitCapsuleInertia();

	PrefabLibrary::AddPrefab("bullet", std::move(bulletPrefab));

	bulletRadius = 0.75f;

	bulletPrefab = std::make_unique<BossBullet>();

	bulletPrefab->SetBoundingVolume((CollisionVolume*) new SphereVolume(bulletRadius, CollisionLayer::EnemyProj));
	bulletPrefab->GetTransform().SetScale(Vector3(bulletRadius));

	bulletPrefab->SetRenderObject(new RenderObject(bulletPrefab->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("sphere"), nullptr));
	bulletPrefab->SetPhysicsObject(new PhysicsObject(&bulletPrefab->GetTransform(), bulletPrefab->GetBoundingVolume(), true));

	bulletPrefab->GetRenderObject()->SetColour(Vector4(0.2f, 1.0f, 0.5f, 1.0f));

	bulletPrefab->GetPhysicsObject()->SetInverseMass(1.0f);
	bulletPrefab->GetPhysicsObject()->SetGravWeight(1.0f);
	bulletPrefab->GetPhysicsObject()->InitCapsuleInertia();

	PrefabLibrary::AddPrefab("bossBullet", std::move(bulletPrefab));
}

void LoadAsset() {
	LoadMenuAsset();
	LoadGlobalAssets();
	LoadAnimationAsset();
	LoadPrefabs();
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
	Window* w = Window::CreateGameWindow("CSC8507 Game technology!", 1280, 720);

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