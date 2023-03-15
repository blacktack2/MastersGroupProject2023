/**
 * @file   TutorialGame.cpp
 * @brief  See TutorialGame.h.
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   March 2023
 */
#include "TutorialGame.h"

#include "AssetLibrary.h"
#include "AssetLoader.h"
#include "PrefabLibrary.h"

#include "DebugViewPoint.h"
#include "GameGridManager.h"
#include "GameStateManager.h"
#include "GameTechRenderer.h"
#include "GameWorld.h"
#include "InputKeyMap.h"
#include "MenuManager.h"
#include "OptionManager.h"

#include "Camera.h"
#include "Boss.h"

#include "BulletInstanceManager.h"
#include "PlayerBullet.h"

#include "GameLevel.h"
#include "PhysicsSystem.h"

#include "AnimatedRenderObject.h"
#include "PaintRenderObject.h"
#include "PhysicsObject.h"
#include "PlayerObject.h"
#include "RenderObject.h"

#include "Sound.h"
#include "SoundSource.h"
#include "SoundSystem.h"
#include "TestAudio.h"
#include "Hud.h"
#include "MeshAnimation.h"

#include "Debug.h"

//#include "./stb/stb_image.h"
#include "XboxControllerManager.h"

#include <string>

using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame() :
debugViewPoint(DebugViewPoint::Instance()), gridManager(GameGridManager::instance()), gameStateManager(GameStateManager::instance()),
renderer(GameTechRenderer::instance()), gameWorld(GameWorld::instance()), keyMap(InputKeyMap::instance()), menuManager(MenuManager::instance()), optionManager(OptionManager::instance()) {
	sunLight = gameWorld.AddLight(new Light({ 0, 0, 0, 0 }, { 1, 1, 1, 1 }, 0, { 0.9f, 0.4f, 0.1f }));

	physics = std::make_unique<PhysicsSystem>(gameWorld);
	hud = std::make_unique<Hud>();
	
	SoundSystem::Initialize();
	StartLevel();
}

TutorialGame::~TutorialGame() {
	gameWorld.ClearAndErase();
	gameWorld.ClearLight();
	BulletInstanceManager::instance().NullifyArray();
	gridManager.Clear();

	SoundSystem::Destroy();
}

void TutorialGame::StartLevel() {
	InitWorld();

	XboxControllerManager::GetXboxController().CheckPorts();
	int numOfPlayers = XboxControllerManager::GetXboxController().GetActiveControllerNumber();

	for (int i = 0; i < 4; i++) players[i] = nullptr;

	players[0] = AddPlayerToWorld(0, Vector3(0, 5, 90));
	keyMap.ChangePlayerControlTypeMap(0, ControllerType::KeyboardMouse);
	for (int i = 1; i <= numOfPlayers; i++) {
		players[i] = AddPlayerToWorld(i, Vector3(0, 5, 90));		// TODO: currently this will result in access violation if 4 controllers are connected
		keyMap.ChangePlayerControlTypeMap(i, ControllerType::Xbox);
	}
	renderer.SetNumPlayers(numOfPlayers + 1);		// +1 accounts for players[0] who uses Keyboard & Mouse
	SetCameraFollow(players[0]);		// Currently set to player[0] is crucial for split screen

	boss = AddBossToWorld({ 0, 5, -20 }, { 2,2,2 }, 1);
	boss->SetNextTarget(players[0]);
}

void TutorialGame::Clear() {
	gameStateManager.SetGameState(GameState::OnGoing);
	gameWorld.ClearAndErase();
	BulletInstanceManager::instance().ObjectIntiation();
	physics->Clear();
	gridManager.Clear();
	boss = nullptr;
}

void TutorialGame::InitWorld() {
	Clear();

	gridManager.AddGameGrid(new GameGrid(Vector3(0.0f), 400, 400, 1));
	BuildLevel();

	InitGameExamples();

	gameWorld.UpdateStaticTree();

	InitCamera();
}

void TutorialGame::UpdateGame(float dt) {
	GameState gameState = gameStateManager.GetGameState();
	keyMap.Update();

	// TODO - This is temporary (remove)
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM1))
		renderer.SetNumPlayers(1);
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM2))
		renderer.SetNumPlayers(2);
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM3))
		renderer.SetNumPlayers(3);
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM4))
		renderer.SetNumPlayers(4);

	//temp change player
	/*if (Window::GetKeyboard()->KeyDown(KeyboardKeys::P))
	{
		if(players[1])
			SetCameraFollow(players[1]);
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::O))
	{
		SetCameraFollow(players[0]);
	}*/

	debugViewPoint.BeginFrame();
	debugViewPoint.MarkTime("Update");
	
	if (optionManager.GetSunMove()) {
		float runtime = gameWorld.GetRunTime();
		sunLight->direction = Vector3(std::sin(runtime), std::cos(runtime), 0.0f);
		renderer.GetSkyboxPass().SetSunDir(sunLight->direction);
	}

	if (!optionManager.GetSound()) {
		SoundSystem::GetSoundSystem()->SetMasterVolume(0.0);
	}
	if (optionManager.GetSound()) {
		SoundSystem::GetSoundSystem()->SetMasterVolume(0.5);
		SoundSystem::GetSoundSystem()->Update(dt);
	}
	if (optionManager.GetVolumeUp()) {
		int i = optionManager.GetUpTimes();
		SoundSystem::GetSoundSystem()->SetMasterVolume(0.5 + i * 0.1);
	}
	if (optionManager.GetVolumeDown()) {
		int i = optionManager.GetDownTimes();
		SoundSystem::GetSoundSystem()->SetMasterVolume(0.5 - i * 0.1);
	}

	UpdateGameCore(dt);

	//gameStateManager.Update(dt);
	ProcessState();

	UpdateHud(dt);		// TODO: update hud for each splitted screen inside RenderFrame

	debugViewPoint.FinishTime("Update");
	debugViewPoint.MarkTime("Render");
	renderer.Render();
	
	Debug::UpdateRenderables(dt);
	debugViewPoint.FinishTime("Render");
}

bool TutorialGame::IsQuit() {
	return gameStateManager.GetGameState() == GameState::Quit;
}

void TutorialGame::UpdateGameCore(float dt) {
	Vector2 screenSize = Window::GetWindow()->GetScreenSize();

	gameWorld.GetMainCamera()->UpdateCamera(dt);

	GameGridManager::instance().Update(dt);

	gameWorld.GetCamera(1)->UpdateCamera(dt);
	gameWorld.GetCamera(2)->UpdateCamera(dt);
	gameWorld.GetCamera(3)->UpdateCamera(dt);
	gameWorld.GetCamera(4)->UpdateCamera(dt);

	Vector3 crossPos = CollisionDetection::Unproject(Vector3(screenSize * 0.5f, 0.99f), *gameWorld.GetMainCamera());
	Debug::DrawAxisLines(Matrix4::Translation(crossPos), 1.0f);

	gameWorld.PreUpdateWorld();

	gameWorld.UpdateWorld(dt);
	renderer.Update(dt);
	physics->Update(dt);

	gameWorld.PostUpdateWorld();
	
	if (gameLevel->GetShelterTimer() > 20.0f) {
		gameLevel->SetShelterTimer(0.0f);
		UpdateLevel();
	}
}

void TutorialGame::UpdateHud(float dt)
{	
	if (players[0]) {
		Debug::Print(std::string("health: ").append(std::to_string((int)players[0]->GetHealth()->GetHealth())), Vector2(5, 5), Vector4(1, 1, 0, 1));
	}
	if (boss) {
		Debug::Print(std::string("Boss health: ").append(std::to_string((int)boss->GetHealth()->GetHealth())), Vector2(60, 5), Vector4(1, 1, 0, 1));
	}

	renderer.SetBossHP((int)boss->GetHealth()->GetHealth());
	for (int i = 0; i < 4; i++) {
		if (players[i]) renderer.SetPlayerHp(i, (int)players[i]->GetHealth()->GetHealth());
	}

	//hud->loadHuds((int)boss->GetHealth()->GetHealth(), (int)players[gameWorld.GetMainCamera()->GetPlayerID()]->GetHealth()->GetHealth());
	//renderer.GetHudRPass().SetHud(hud->getHuds());
}

void TutorialGame::ProcessState() {
	NCL::InputKeyMap& keyMap = NCL::InputKeyMap::instance();
	if (keyMap.GetButton(InputType::Restart)) {
		this->StartLevel();
	}
	if (keyMap.GetButton(InputType::Return)) {
		gameStateManager.SetGameState(GameState::Quit);
	}
}

void TutorialGame::InitCamera() {
	gameWorld.InitCameras();
}

void TutorialGame::InitGameExamples() {
	InitDefaultFloor();
	SoundSystem::GetSoundSystem()->SetMasterVolume(1.0f);
}

void TutorialGame::InitDefaultFloor() {
	AddFloorToWorld(Vector3(0, -2, 0));
}

GameObject* TutorialGame::AddFloorToWorld(const Vector3& position) {
	GameObject* floor = new GameObject("Floor");

	Vector3 floorSize = Vector3(200, 2, 200);
	AABBVolume* volume = new AABBVolume(floorSize, CollisionLayer::PaintAble);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position);
	
	PaintRenderObject* render = new PaintRenderObject(floor->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("cube"), nullptr);
	floor->SetRenderObject(render);

	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetStatic();
	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	gameWorld.AddGameObject(floor);

	return floor;
}

PlayerObject* TutorialGame::AddPlayerToWorld(int playerID, const Vector3& position) {
	static int id = 0;

	PlayerObject* character = new PlayerObject(playerID);
	SphereVolume* volume = new SphereVolume(1.0f, CollisionLayer::Player);

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(1, 1, 1))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(character->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("goat"), nullptr));
	
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetRenderObject()->SetColour(Vector4(1, 0.9f, 0.8f, 1));

	character->GetPhysicsObject()->SetInverseMass(1);
	character->GetPhysicsObject()->InitSphereInertia();

	gameWorld.AddGameObject(character);

	return character;
}

void TutorialGame::SetCameraFollow(PlayerObject* p)
{
	int id = p->GetPlayerID();
	if (id == 0) gameWorld.SetMainCamera(4);
	else gameWorld.SetMainCamera(id);
	//gameWorld.GetMainCamera()->SetFollow(&(p->GetTransform()));
}

Boss* TutorialGame::AddBossToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	Boss* boss = new Boss();

	boss->SetBoundingVolume((CollisionVolume*)new AABBVolume(dimensions));

	boss->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	boss->SetRenderObject(new AnimatedRenderObject(boss->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("boss"), AssetLibrary<MeshMaterial>::GetAsset("boss"), AssetLibrary<MeshAnimation>::GetAsset("WalkForward")));

	boss->GetRenderObject()->SetColour({ 1, 1, 1, 1 });
	boss->SetPhysicsObject(new PhysicsObject(&boss->GetTransform(), boss->GetBoundingVolume()));

	boss->GetPhysicsObject()->SetInverseMass(inverseMass);
	boss->GetPhysicsObject()->InitCubeInertia();

	gameWorld.AddGameObject(boss);

	return boss;
}

void TutorialGame::BuildLevel() {
	interval = 5.0f;

	gameLevel = new GameLevel();
	gameLevel->AddRectanglarLevel("BasicLevel.txt", { -200, 0, -200 }, interval);
	gameWorld.AddGameObject(gameLevel);

	UpdateLevel();
}

void TutorialGame::UpdateLevel() {
	for (auto& object : gameLevel->GetGameStuffs()){
		if (object->HasDestroyed()){
			object->Destroy(false);
			if (object->objectType == ObjectType::Pillar){
				Vector3 dimensions{ interval / 2.0f, 10, interval / 2.0f };
				Obstacle* pillar = new Obstacle{ object, true };

				pillar->SetBoundingVolume((CollisionVolume*)new AABBVolume(dimensions * Vector3{ 1.3f, 2.0f, 1.3f }, CollisionLayer::PaintAble));

				pillar->GetTransform()
					.SetPosition(object->worldPos + Vector3{ 0,20,0 })
					.SetScale(dimensions * 2);
				//pillar->SetRenderObject(new RenderObject(&pillar->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("pillar"), healingKitTex, nullptr));
				
				PaintRenderObject* render = new PaintRenderObject(pillar->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("pillar"), AssetLibrary<MeshMaterial>::GetAsset("pillar"));
				pillar->SetRenderObject(render);
				
				pillar->SetPhysicsObject(new PhysicsObject(&pillar->GetTransform(), pillar->GetBoundingVolume()));
				pillar->GetPhysicsObject()->SetInverseMass(0);
				pillar->GetPhysicsObject()->InitCubeInertia();
				gameWorld.AddGameObject(pillar);
			}
			if (object->objectType == ObjectType::FenceX){
				Vector3 dimensions{ interval / 4.0f, 0.5f, interval / 5.0f };
				Obstacle* fenceX = new Obstacle{ object, true };
				fenceX->SetBoundingVolume((CollisionVolume*)new AABBVolume(dimensions * 2, CollisionLayer::PaintAble));
				fenceX->GetTransform()
					.SetPosition(object->worldPos + Vector3{ 0,2,0 })
					.SetScale(dimensions * 2);
				//fenceX->SetRenderObject(new RenderObject(&fenceX->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("fenceX"), basicTex, nullptr));		// TODO: change to the right Mesh
				
				PaintRenderObject* render = new PaintRenderObject(fenceX->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("fenceX"), nullptr);
				fenceX->SetRenderObject(render);

				fenceX->SetPhysicsObject(new PhysicsObject(&fenceX->GetTransform(), fenceX->GetBoundingVolume()));
				fenceX->GetPhysicsObject()->SetInverseMass(0);
				fenceX->GetPhysicsObject()->InitCubeInertia();
				gameWorld.AddGameObject(fenceX);
			}
			if (object->objectType == ObjectType::FenceY) {
				Vector3 dimensions{ interval / 5.0f, 0.5f, interval / 4.0f };
				Obstacle* fenceY = new Obstacle{ object, true };
				fenceY->SetBoundingVolume((CollisionVolume*)new AABBVolume(dimensions * 2, CollisionLayer::PaintAble));
				fenceY->GetTransform()
					.SetPosition(object->worldPos + Vector3{ 0,2,0 })
					.SetScale(dimensions * 2);
				//fenceY->SetRenderObject(new RenderObject(&fenceY->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("fenceY"), basicTex, nullptr));		// TODO: change to the right Mesh
				
				PaintRenderObject* render = new PaintRenderObject(fenceY->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("fenceY"), nullptr);
				fenceY->SetRenderObject(render);
				
				fenceY->SetPhysicsObject(new PhysicsObject(&fenceY->GetTransform(), fenceY->GetBoundingVolume()));
				fenceY->GetPhysicsObject()->SetInverseMass(0);
				fenceY->GetPhysicsObject()->InitCubeInertia();
				gameWorld.AddGameObject(fenceY);
			}
			if (object->objectType == ObjectType::Shelter) {
				Vector3 dimensions{ interval / 5.0f, 2.0f, interval / 2.0f };
				Obstacle* shelter = new Obstacle{ object, false };
				shelter->SetBoundingVolume((CollisionVolume*)new AABBVolume(dimensions, CollisionLayer::PaintAble));
				shelter->GetTransform()
					.SetPosition(object->worldPos + Vector3{ 0.0f, 2.2f, 0.0f })
					.SetScale(dimensions);
				//shelter->SetRenderObject(new RenderObject(&shelter->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("shelter"), basicTex, nullptr));

				PaintRenderObject* render = new PaintRenderObject(shelter->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("shelter"), nullptr);
				shelter->SetRenderObject(render);

				shelter->SetPhysicsObject(new PhysicsObject(&shelter->GetTransform(), shelter->GetBoundingVolume()));
				shelter->GetPhysicsObject()->SetInverseMass(0);
				shelter->GetPhysicsObject()->InitCubeInertia();
				gameWorld.AddGameObject(shelter);
			}
		}
	}
}
