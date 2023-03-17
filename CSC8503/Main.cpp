/**
 * @file   Main.cpp
 * @brief  
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#ifdef x64
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
#include "OGLLoadingManager.h"


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

	AssetLibrary<TextureBase>::AddAsset("defaultAlbedo", std::move(AssetLoader::LoadTexture("DefaultAlbedo.png")));
	AssetLibrary<TextureBase>::AddAsset("defaultBump", std::move(AssetLoader::LoadTexture("DefaultBump.png")));
	AssetLibrary<TextureBase>::AddAsset("defaultSpec", std::move(AssetLoader::LoadTexture("DefaultSpec.png")));
	AssetLibrary<TextureBase>::AddAsset("basic", std::move(AssetLoader::LoadTexture("checkerboard.png")));
	AssetLibrary<TextureBase>::AddAsset("pillarTex", std::move(AssetLoader::LoadTexture("pillarTex.jpg")));

	AssetLibrary<MeshMaterial>::AddAsset("default", std::make_shared<MeshMaterial>("Default.mat"));
	AssetLibrary<MeshMaterial>::AddAsset("pillar", std::make_shared<MeshMaterial>("pillar.mat"));
	AssetLibrary<MeshMaterial>::AddAsset("floor", std::make_shared<MeshMaterial>("Floor.mat"));

	AssetLibrary<ShaderBase>::AddAsset("modelDefault", std::move(AssetLoader::CreateShader("modelDefault.vert", "modelDefault.frag")));
	AssetLibrary<ShaderBase>::AddAsset("paintDefault", std::move(AssetLoader::CreateShader("modelDefault.vert", "modelPaintTexture.frag")));
	AssetLibrary<ShaderBase>::AddAsset("animationDefault", std::move(AssetLoader::CreateShader("modelAnimated.vert", "modelAnimated.frag")));
	AssetLibrary<ShaderBase>::AddAsset("shadowDefault", std::move(AssetLoader::CreateShader("shadowDefault.vert", "shadowDefault.frag")));
}

void LoadMenuAsset() {
	std::cout << "Loading menu assets\n";
	AssetLibrary<TextureBase>::AddAsset("menuMain", AssetLoader::LoadTexture("Menu/defaultMain.jpg"));
	AssetLibrary<TextureBase>::AddAsset("menuPause", AssetLoader::LoadTexture("Menu/defaultpause.jpg"));
	AssetLibrary<TextureBase>::AddAsset("menuOption", AssetLoader::LoadTexture("Menu/defaultOption.jpg"));

	AssetLibrary<TextureBase>::AddAsset("button0", AssetLoader::LoadTexture("Menu/button0.jpg"));
	AssetLibrary<TextureBase>::AddAsset("button1", AssetLoader::LoadTexture("Menu/button1.jpg"));
	AssetLibrary<TextureBase>::AddAsset("button2", AssetLoader::LoadTexture("Menu/button2.jpg"));
	AssetLibrary<TextureBase>::AddAsset("button3", AssetLoader::LoadTexture("Menu/button3.jpg"));
	AssetLibrary<TextureBase>::AddAsset("button4", AssetLoader::LoadTexture("Menu/button4.jpg"));
	AssetLibrary<TextureBase>::AddAsset("button5", AssetLoader::LoadTexture("Menu/button5.jpg"));
	AssetLibrary<TextureBase>::AddAsset("button6", AssetLoader::LoadTexture("Menu/button6.jpg"));
	AssetLibrary<TextureBase>::AddAsset("button7", AssetLoader::LoadTexture("Menu/button7.jpg"));

	AssetLibrary<TextureBase>::AddAsset("buttonSlide0", AssetLoader::LoadTexture("Menu/buttonSlide0.jpg"));
	AssetLibrary<TextureBase>::AddAsset("buttonSlide1", AssetLoader::LoadTexture("Menu/buttonSlide1.jpg"));
	AssetLibrary<TextureBase>::AddAsset("buttonSlide2", AssetLoader::LoadTexture("Menu/buttonSlide2.jpg"));
	AssetLibrary<TextureBase>::AddAsset("buttonSlide3", AssetLoader::LoadTexture("Menu/buttonSlide3.jpg"));
	AssetLibrary<TextureBase>::AddAsset("buttonSlide4", AssetLoader::LoadTexture("Menu/buttonSlide4.jpg"));
	AssetLibrary<TextureBase>::AddAsset("buttonSlide5", AssetLoader::LoadTexture("Menu/buttonSlide5.jpg"));
	AssetLibrary<TextureBase>::AddAsset("buttonSlide6", AssetLoader::LoadTexture("Menu/buttonSlide6.jpg"));
	AssetLibrary<TextureBase>::AddAsset("buttonSlide7", AssetLoader::LoadTexture("Menu/buttonSlide7.jpg"));

	AssetLibrary<TextureBase>::AddAsset("checkbox0", AssetLoader::LoadTexture("Menu/checkbox.jpg"));
	AssetLibrary<TextureBase>::AddAsset("checkbox1", AssetLoader::LoadTexture("Menu/checkboxmark.jpg"));
	AssetLibrary<TextureBase>::AddAsset("checkboxSlide0", AssetLoader::LoadTexture("Menu/checkboxslide0.jpg"));
	AssetLibrary<TextureBase>::AddAsset("checkboxSlide1", AssetLoader::LoadTexture("Menu/checkboxslide1.jpg"));
	AssetLibrary<TextureBase>::AddAsset("plus", AssetLoader::LoadTexture("Menu/plus.jpg"));
	AssetLibrary<TextureBase>::AddAsset("plusslide", AssetLoader::LoadTexture("Menu/plusslide.jpg"));
	AssetLibrary<TextureBase>::AddAsset("minus", AssetLoader::LoadTexture("Menu/minus.jpg"));
	AssetLibrary<TextureBase>::AddAsset("minusslide", AssetLoader::LoadTexture("Menu/minusslide.jpg"));
	AssetLibrary<TextureBase>::AddAsset("return", AssetLoader::LoadTexture("Menu/return.jpg"));
	AssetLibrary<TextureBase>::AddAsset("returnSlide", AssetLoader::LoadTexture("Menu/returnslide.jpg"));

	AssetLibrary<TextureBase>::AddAsset("num0", AssetLoader::LoadTexture("Menu/num0.jpg"));
	AssetLibrary<TextureBase>::AddAsset("num1", AssetLoader::LoadTexture("Menu/num1.jpg"));
	AssetLibrary<TextureBase>::AddAsset("num2", AssetLoader::LoadTexture("Menu/num2.jpg"));
	AssetLibrary<TextureBase>::AddAsset("num3", AssetLoader::LoadTexture("Menu/num3.jpg"));
	AssetLibrary<TextureBase>::AddAsset("num4", AssetLoader::LoadTexture("Menu/num4.jpg"));
	AssetLibrary<TextureBase>::AddAsset("num5", AssetLoader::LoadTexture("Menu/num5.jpg"));
	AssetLibrary<TextureBase>::AddAsset("num6", AssetLoader::LoadTexture("Menu/num6.jpg"));
	AssetLibrary<TextureBase>::AddAsset("num7", AssetLoader::LoadTexture("Menu/num7.jpg"));
	AssetLibrary<TextureBase>::AddAsset("num8", AssetLoader::LoadTexture("Menu/num8.jpg"));
	AssetLibrary<TextureBase>::AddAsset("num9", AssetLoader::LoadTexture("Menu/num9.jpg"));

	AssetLibrary<TextureBase>::AddAsset("fontAtlas", std::move(AssetLoader::LoadTexture("PressStart2P.png")));
	AssetLibrary<TextureBase>::AddAsset("BossHealthBarBorder", std::move(AssetLoader::LoadTexture("HP/Borders/Border_Style_3.png")));
	AssetLibrary<TextureBase>::AddAsset("BossHealthBar", std::move(AssetLoader::LoadTexture("HP/Style_3.png")));

	AssetLibrary<TextureBase>::AddAsset("PlayerHealthBarBorder", std::move(AssetLoader::LoadTexture("HP/Borders/Border_Style_4.png")));
	AssetLibrary<TextureBase>::AddAsset("PlayerHealthBar", std::move(AssetLoader::LoadTexture("HP/Style_2.png")));

	AssetLibrary<ShaderBase>::AddAsset("menu", std::move(AssetLoader::CreateShader("menuVertex.vert", "menuFragment.frag")));
	AssetLibrary<ShaderBase>::AddAsset("button", std::move(AssetLoader::CreateShader("buttonVertex.vert", "buttonFragment.frag")));
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

	AssetLibrary<MeshGeometry>::AddAsset("player", AssetLoader::LoadMesh("Player/Player.msh"));

	AssetLibrary<MeshMaterial>::AddAsset("player", std::make_shared<MeshMaterial>("Player/Player.mat"));

	AssetLibrary<MeshAnimation>::AddAsset("PlayerJump", std::make_shared<MeshAnimation>("Player/Jump.anm"));
	AssetLibrary<MeshAnimation>::AddAsset("PlayerIdle", std::make_shared<MeshAnimation>("Player/PlayerIdle.anm"));
	AssetLibrary<MeshAnimation>::AddAsset("PlayerForward", std::make_shared<MeshAnimation>("Player/PlayerForward.anm"));
	AssetLibrary<MeshAnimation>::AddAsset("PlayerBackward", std::make_shared<MeshAnimation>("Player/PlayerBackward.anm"));
	AssetLibrary<MeshAnimation>::AddAsset("PlayerLeft", std::make_shared<MeshAnimation>("Player/PlayerLeft.anm"));
	AssetLibrary<MeshAnimation>::AddAsset("PlayerRight", std::make_shared<MeshAnimation>("Player/PlayerRight.anm"));
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
#endif // x64

#ifdef _ORBIS
#include <stdlib.h>
unsigned int sceLibcHeapExtendedAlloc = 1;			/* Switch to dynamic allocation */
size_t       sceLibcHeapSize = 256 * 1024 * 1024;	/* Set up heap area upper limit as 256 MiB */

#include "../Plugins/PlayStation4/PS4Window.h"
#include "../Plugins/PlayStation4/Ps4AudioSystem.h"
#include "../Plugins/PlayStation4/PS4InputManager.h"
#include "PS4TutorialGame.h"

#include <iostream>

using namespace NCL;
using namespace NCL::PS4;

#endif // _ORBIS


int main() {

#ifdef x64
	Window* w = Window::CreateGameWindow("CSC8507 Game technology!", 1280, 720);

	GameTechRenderer& renderer = GameTechRenderer::instance();
	OGLLoadingManager loadingScreen = OGLLoadingManager(w, renderer);


	std::cout << "loading\n";

	loadingScreen.Load(LoadAsset);
	renderer.InitPipeline();

	if (!w->HasInitialised()) {
		return -1;
	}

	StartPushdownAutomata(w);

	Window::DestroyGameWindow();
#endif //x64

#ifdef _ORBIS
	PS4Window* w = (PS4Window*)Window::CreateGameWindow("PS4 Example Code", 1920, 1080);

	/*ExampleRenderer renderer(w);*/
	//
	//PS4Input		input		= PS4Input();

	PS4::PS4InputManager::Initialize();

	/*renderer.CreateCamera(&input);*/

	TutorialGame* game = new TutorialGame();

	Ps4AudioSystem* audioSystem = new Ps4AudioSystem(8);

	GameTimer t;

	while (w->UpdateWindow()) {
		float time = w->GetTimer()->GetTimeDeltaSeconds();

		//input.Poll();

		PS4InputManager::Update();

		game->UpdateGame(time);

		/*renderer.Update(time);
		renderer.Render();*/
		//Player1
		if (PS4InputManager::GetButtons(0, 0)) {
			std::cout << "Player 1:Triangle button pressed" << std::endl;
		}
		if (PS4InputManager::GetButtons(0, 1)) {
			std::cout << "Player 1: Circle button pressed" << std::endl;
			return 1;
		}

		//Player2
		if (PS4InputManager::GetButtons(1, 0)) {
			std::cout << "Player 2:Triangle button pressed" << std::endl;
		}
		if (PS4InputManager::GetButtons(1, 1)) {
			std::cout << "Player 2:Circle button pressed" << std::endl;
			return 1;
		}


		//if (input.GetButton(0)) {
		//	std::cout << "LOL BUTTON" << std::endl;
		//}

		//if (input.GetButton(1)) {
		//	return 1;
		//}
	}

	delete audioSystem;
	PS4InputManager::Destroy();
	return 1;
#endif
}