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

#include "Light.h"
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
	sunLight = &gameWorld.AddDirectionalLight(Vector3(0.9f, 0.4f, 0.1f), Vector4(1.0f));

	physics = std::make_unique<PhysicsSystem>(gameWorld);
	
	SoundSystem::Initialize();
	StartLevel();
	gameStateManager.SetIsNetworked(false);
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
	numOfPlayers = XboxControllerManager::GetXboxController().GetActiveControllerNumber();
	if (numOfPlayers > 4)
		numOfPlayers = 4;
	boss = AddBossToWorld({ 0, 5, -20 }, Vector3(4), 2);
	// A messy way of spawning multiple player and 
	AddPlayer(0, ControllerType::KeyboardMouse);
	for (int i = 1; i <= numOfPlayers; i++) {
		AddPlayer(i, ControllerType::Xbox);
	}
	numOfPlayers++;		// +1 accounts for players[0] who uses Keyboard & Mouse
	playerNum = numOfPlayers;
	renderer.SetNumPlayers(playerNum);
	gameWorld.SetScreenNum(playerNum);
	SetCameraFollow(players[0]);		// Currently set to player[0] is crucial for split screen

	boss -> SetNextTarget(players[0]);
}

void TutorialGame::Clear() {
	for (int i = 0; i < playerNum; i++) {
		players[i] = nullptr;
	}
	playerNum = 0;
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
	debugViewPoint.BeginFrame();

	GameState gameState = gameStateManager.GetGameState();
	keyMap.Update();

	// TODO - This is temporary (remove)
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM1)) {
		renderer.SetNumPlayers(1);
		gameWorld.SetScreenNum(1);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM2)) {
		renderer.SetNumPlayers(2);
		gameWorld.SetScreenNum(2);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM3)) {
		renderer.SetNumPlayers(3);
		gameWorld.SetScreenNum(3);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM4)) {
		renderer.SetNumPlayers(4);
		gameWorld.SetScreenNum(4);
	}
	
	if (optionManager.GetSunMove()) {
		float runtime = gameWorld.GetRunTime();
		sunLight->SetDirection(Vector3(std::sin(runtime), std::cos(runtime), 0.0f));
		renderer.SetSunDir(sunLight->GetDirection());
	}
	debugViewPoint.MarkTime("Update Sound");

	if (!optionManager.GetSound()) {
		SoundSystem::GetSoundSystem()->SetMasterVolume(0.0);
	}
	if (optionManager.GetSound()) {
		//SoundSystem::GetSoundSystem()->SetMasterVolume(0.5);
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
	debugViewPoint.FinishTime("Update Sound");

	
	UpdateGameCore(dt);

	gameStateManager.Update(dt);
	ProcessState();

	
	//debugViewPoint.MarkTime("Render");
	renderer.Render();
	
	Debug::UpdateRenderables(dt);
	//debugViewPoint.FinishTime("Render");
}

bool TutorialGame::IsQuit() {
	return gameStateManager.GetGameState() == GameState::Quit;
}

void TutorialGame::UpdateGameCore(float dt) {
	GameGridManager::instance().Update(dt);
	if (boss) {
		BossTarget();
	}

	gameWorld.PreUpdateWorld();
	debugViewPoint.MarkTime("Game Update");
	gameWorld.UpdateWorld(dt);
	debugViewPoint.FinishTime("Game Update");
	debugViewPoint.MarkTime("Physics Update");
	physics->Update(dt);
	debugViewPoint.FinishTime("Physics Update");

	gameWorld.PostUpdateWorld();
	
	if (gameLevel->GetShelterTimer() > 20.0f) {
		gameLevel->SetShelterTimer(0.0f);
		UpdateLevel();
	}
	gameWorld.UpdateCamera(dt);
}

void TutorialGame::BossTarget() {
	//change boss target
	Vector3 displacement;
	PlayerObject* target = boss->GetTarget();
	float mindist = 300;
	for (int i = 0; i < playerNum; i++) {
		if (players[i]->GetHealth()->GetHealth() <= 0) {
			continue;
		}
		displacement = players[i]->GetTransform().GetGlobalPosition() - boss->GetTransform().GetGlobalPosition();
		float dist = abs(displacement.Length());
		if (dist < mindist) {
			target = players[i];
			mindist = dist;
		}
	}
	boss->SetNextTarget(target);
}

void TutorialGame::ProcessState() {
	if (gameStateManager.GetGameState() == GameState::OnGoing) {
		float totalHealth = 0;
		for (int i = 0; i < playerNum; i++) {
			totalHealth += players[i]->GetHealth()->GetHealth();
		}
		if (totalHealth <= 0) {
			gameStateManager.SetGameState(GameState::Lose);
		}
	}

	switch (gameStateManager.GetGameState()) {
	case GameState::OnGoing:
		// TODO - makeshift crosshair
		Debug::Print("+", Vector2(49.5f, 49.5f), Vector4(1, 1, 1, 1));
		break;
	case GameState::Win:
		Debug::Print("You Win!", Vector2(5.0f, 70.0f), Debug::GREEN);
		Debug::Print("Press [R] or [Start] to play again", Vector2(5, 80), Debug::WHITE);
		break;
	case GameState::Lose:
		Debug::Print("You Lose!", Vector2(5.0f, 70.0f), Debug::RED);
		Debug::Print("Press [R] or [Start] to play again", Vector2(5, 80), Debug::WHITE);
		break;
	}

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
	AddBackGroundToWorld(Vector3(0,-2, 0));
	AddFloorToWorld(Vector3(0, -2, 0));
	AddWallXToWorld(Vector3(-100, 0, -200));
	AddWallXToWorld(Vector3(100, 0, -200));
	AddWallXToWorld(Vector3(-100, 0, 200));
	AddWallXToWorld(Vector3(100, 0, 200));
	AddWallYToWorld(Vector3(-200, 0, -100));
	AddWallYToWorld(Vector3(200, 0, -100));
	AddWallYToWorld(Vector3(-200, 0, 100));
	AddWallYToWorld(Vector3(200, 0, 100));
}
GameObject* TutorialGame::AddBackGroundToWorld(const Vector3& position) {
	GameObject* backGround = new GameObject("BackGround");
	backGround->GetTransform()
		.SetScale(Vector3(10, 10, 10))
		.SetPosition(position);
	backGround->SetRenderObject(new PaintRenderObject(backGround->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("mountMesh"), AssetLibrary<MeshMaterial>::GetAsset("mountMat")));
	gameWorld.AddGameObject(backGround);
	return backGround;
}
GameObject* TutorialGame::AddWallXToWorld(const Vector3& position) {
	GameObject* wall = new GameObject("wallX");
	Vector3 wallSize = Vector3(1, 1, 1);
	wall->GetTransform()
		.SetScale(Vector3(1.566f, 1.7f, 2))
		.SetPosition(position);
	wall->SetRenderObject(new PaintRenderObject(wall->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("wallX"), AssetLibrary<MeshMaterial>::GetAsset("wall")));
	AABBVolume* volume = new AABBVolume(Vector3{ 100.0f, 50.0f, 1.0f }, CollisionLayer::PaintAble);
	wall->SetBoundingVolume((CollisionVolume*)volume);
	wall->SetPhysicsObject(new PhysicsObject(&wall->GetTransform(), wall->GetBoundingVolume()));
	wall->GetPhysicsObject()->SetInverseMass(0);
	wall->GetPhysicsObject()->InitCubeInertia();
	gameWorld.AddGameObject(wall);
	return wall;
}
GameObject* TutorialGame::AddWallYToWorld(const Vector3& position) {
	GameObject* wall = new GameObject("wallY");
	Vector3 wallSize = Vector3(1, 1, 1);
	wall->GetTransform()
		.SetScale(Vector3(2.0f, 1.7f, 1.566f))
		.SetPosition(position);
	wall->SetRenderObject(new PaintRenderObject(wall->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("wallY"), AssetLibrary<MeshMaterial>::GetAsset("wall")));
	AABBVolume* volume = new AABBVolume(Vector3{ 1.0f, 50.0f, 100.0f }, CollisionLayer::PaintAble);
	wall->SetBoundingVolume((CollisionVolume*)volume);
	wall->SetPhysicsObject(new PhysicsObject(&wall->GetTransform(), wall->GetBoundingVolume()));
	wall->GetPhysicsObject()->SetInverseMass(0);
	wall->GetPhysicsObject()->InitCubeInertia();
	gameWorld.AddGameObject(wall);
	return wall;
}
GameObject* TutorialGame::AddFloorToWorld(const Vector3& position) {
	GameObject* floor = new GameObject("Floor");

	Vector3 floorSize = Vector3(200, 2, 200);
	AABBVolume* volume = new AABBVolume(floorSize, CollisionLayer::PaintAble);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position);
	
	PaintRenderObject* render = new PaintRenderObject(floor->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("cube"), AssetLibrary<MeshMaterial>::GetAsset("floor"));
	render->SetTexScale(Vector2(10.0f));
	floor->SetRenderObject(render);

	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetStatic();
	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	gameWorld.AddGameObject(floor);

	return floor;
}

void TutorialGame::AddPlayer(int index, ControllerType type) {
	players[index] = AddPlayerToWorld(index, Vector3(0, 5, 90));
	keyMap.ChangePlayerControlTypeMap(index, type);
	players[index]->GetCamera()->GetHud().AddHealthBar(players[index]->GetHealth(), Vector2(-0.6f, 0.9f), Vector2(0.35f, 0.03f));
	players[index]->GetCamera()->GetHud().SetPlayerHealth(players[index]->GetHealth());
	players[index]->GetCamera()->GetHud().AddHealthBar(boss->GetHealth(), Vector2(0.0f, -0.8f), Vector2(0.7f, 0.04f));
}

PlayerObject* TutorialGame::AddPlayerToWorld(int playerID, const Vector3& position) {
	static int id = 0;

	PlayerObject* character = new PlayerObject(playerID);
	CapsuleVolume* volume = new CapsuleVolume(1.2f, 0.2f, CollisionLayer::Player);

	character->SetBoundingVolume(volume);

	float scale = 2.0f;
	character->GetTransform()
		.SetScale(Vector3(scale))
		.SetPosition(position);

	character->SetRenderObject(new AnimatedRenderObject(character->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("player"), AssetLibrary<MeshMaterial>::GetAsset("player"), AssetLibrary<MeshAnimation>::GetAsset("PlayerIdle")));
	
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
	gameWorld.SetMainCamera(id);
	//gameWorld.GetMainCamera()->SetFollow(&(p->GetTransform()));
}

Boss* TutorialGame::AddBossToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	Boss* boss = new Boss();

	//boss->SetBoundingVolume((CollisionVolume*)new AABBVolume(dimensions));
	boss->SetBoundingVolume((CollisionVolume*)new AABBVolume(Vector3{ dimensions.x,dimensions.y*2.2f,dimensions.z}));

	boss->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions*2);

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
				Vector3 dimensions{ interval / 2.0f, 15, interval / 2.0f };
				Obstacle* pillar = new Obstacle{ object, true };

				pillar->SetBoundingVolume((CollisionVolume*)new AABBVolume(dimensions * Vector3{ 1.3f, 2.0f, 1.3f }, CollisionLayer::PaintAble));

				pillar->GetTransform()
					.SetPosition(object->worldPos + Vector3{ 0,15,0 })
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
				Vector3 dimensions{ interval / 2.0f, 3.0f, interval / 5.0f };
				Obstacle* fenceX = new Obstacle{ object, true };
				fenceX->SetBoundingVolume((CollisionVolume*)new AABBVolume(dimensions*2, CollisionLayer::PaintAble));
				fenceX->GetTransform()
					.SetPosition(object->worldPos + Vector3{ 0,3,0 })
					.SetScale(dimensions * 2);
				//fenceX->SetRenderObject(new RenderObject(&fenceX->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("fenceX"), basicTex, nullptr));		// TODO: change to the right Mesh
				
				PaintRenderObject* render = new PaintRenderObject(fenceX->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("fenceX"), AssetLibrary<MeshMaterial>::GetAsset("fenceX"));
				fenceX->SetRenderObject(render);

				fenceX->SetPhysicsObject(new PhysicsObject(&fenceX->GetTransform(), fenceX->GetBoundingVolume()));
				fenceX->GetPhysicsObject()->SetInverseMass(0);
				fenceX->GetPhysicsObject()->InitCubeInertia();
				gameWorld.AddGameObject(fenceX);
			}
			if (object->objectType == ObjectType::FenceY) {
				Vector3 dimensions{ interval / 5.0f, 3.0f, interval / 2.0f };
				Obstacle* fenceY = new Obstacle{ object, true };
				fenceY->SetBoundingVolume((CollisionVolume*)new AABBVolume(dimensions * 2, CollisionLayer::PaintAble));
				fenceY->GetTransform()
					.SetPosition(object->worldPos + Vector3{ 0,3,0 })
					.SetScale(dimensions * 2);
				//fenceY->SetRenderObject(new RenderObject(&fenceY->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("fenceY"), basicTex, nullptr));		// TODO: change to the right Mesh
				
				PaintRenderObject* render = new PaintRenderObject(fenceY->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("fenceY"), AssetLibrary<MeshMaterial>::GetAsset("fenceY"));
				fenceY->SetRenderObject(render);
				
				fenceY->SetPhysicsObject(new PhysicsObject(&fenceY->GetTransform(), fenceY->GetBoundingVolume()));
				fenceY->GetPhysicsObject()->SetInverseMass(0);
				fenceY->GetPhysicsObject()->InitCubeInertia();
				gameWorld.AddGameObject(fenceY);
			}
			if (object->objectType == ObjectType::Shelter) {
				Vector3 dimensions{ interval / 1.0f, 5.0f, interval / 1.0f };
				Obstacle* shelter = new Obstacle{ object, false };
				shelter->SetBoundingVolume((CollisionVolume*)new AABBVolume(dimensions*Vector3{0.5f,0.8f,1.0f}, CollisionLayer::PaintAble));
				shelter->GetTransform()
					.SetPosition(object->worldPos + Vector3{ 0.0f,2.7f, 0.0f })
					.SetScale(dimensions);
				//shelter->SetRenderObject(new RenderObject(&shelter->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("shelter"), basicTex, nullptr));

				PaintRenderObject* render = new PaintRenderObject(shelter->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("shelter"), AssetLibrary<MeshMaterial>::GetAsset("shelter"));
				shelter->SetRenderObject(render);

				shelter->SetPhysicsObject(new PhysicsObject(&shelter->GetTransform(), shelter->GetBoundingVolume()));
				shelter->GetPhysicsObject()->SetInverseMass(0);
				shelter->GetPhysicsObject()->InitCubeInertia();
				gameWorld.AddGameObject(shelter);
			}
		}
	}
}
