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

#include "PlayerBullet.h"
#include "Debug.h"

#include "GameWorld.h"
#include "GameGridManager.h"
#include "InkEffectManager.h"
#include "Maze.h"
#include "OrientationConstraint.h"
#include "PhysicsObject.h"
#include "PlayerObject.h"
#include "PositionConstraint.h"
#include "RenderObject.h"
#include "StateGameObject.h"

#include "obstacle.h"
#include "PaintRenderObject.h"
#include "BulletInstanceManager.h"

// testing animation
#include "AnimatedRenderObject.h"
#include "./stb/stb_image.h"

//Audio Testing

#include "Sound.h"
#include "SoundSource.h"
#include "SoundSystem.h"
#include "TestAudio.h"


#include <string>

#include "XboxController.h"

using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame() {
	gameStateManager = &GameStateManager::instance();
	menuManager = &MenuManager::instance();

	world = &GameWorld::instance();
	sunLight = world->AddLight(new Light({ 0, 0, 0, 0 }, { 1, 1, 1, 1 }, 0, { 0.9f, 0.4f, 0.1f }));
	world->AddLight(new Light({ 0.0f, 5.0f, -10.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }, 10.0f));
#ifdef USEVULKAN
	renderer = new GameTechVulkanRenderer(*world);
#else 
	renderer = &GameTechRenderer::instance();
#endif

	physics		= new PhysicsSystem(*world);

	forceMagnitude	= 10.0f;
	inSelectionMode = false;
	debugViewPoint = &NCL::DebugViewPoint::Instance();
	
	SoundSystem::Initialize();
	gridManager = &GameGridManager::instance();
	StartLevel();
}

TutorialGame::~TutorialGame() {
	world->ClearAndErase();
	BulletInstanceManager::instance().NullifyArray();
	gridManager->Clear();
	delete testingBossBehaviorTree;
	delete physics;

	delete[] mazes;

	SoundSystem::Destroy();
}

void TutorialGame::StartLevel() {
	InitWorld();
	player = AddPlayerToWorld(Vector3(0, 5, 90),true);

	testingBoss = AddBossToWorld({ 0, 5, -20 }, { 2,2,2 }, 1);
	testingBossBehaviorTree = new BossBehaviorTree(testingBoss);
	testingBossBehaviorTree->ChangeTarget(player);
}

void TutorialGame::Clear() {
	gameStateManager->SetGameState(GameState::OnGoing);
	delete[] mazes;
	mazes = nullptr;
	world->ClearAndErase();
	BulletInstanceManager::instance().ObjectIntiation();
	physics->Clear();
	gridManager->Clear();
	testingBoss = nullptr;
	delete testingBossBehaviorTree;
	testingBossBehaviorTree = nullptr;
}

void TutorialGame::InitWorld() {
	Clear();

	gridManager->AddGameGrid(new GameGrid({ 0,0,0 }, 300, 300, 2));
	BuildLevel();

	InitGameExamples();

	world->UpdateStaticTree();

	InitCamera();
}

void TutorialGame::UpdateGame(float dt) {
	GameState gameState = gameStateManager->GetGameState();
	keyMap.Update();

	/*XboxController c;
	float v;
	bool b = c.GetRightTrigger(1, v);
	if (b) std::cout << v << "\n";
	else std::cout << "No displacement\n";*/

	debugViewPoint->BeginFrame();
	debugViewPoint->MarkTime("Update");
	
	static bool moveSun = false;
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM0)) {
		moveSun = !moveSun;
	}
	if (moveSun) {
		float runtime = world->GetRunTime();
		sunLight->direction = Vector3(std::sin(runtime), std::cos(runtime), 0.0f);
		renderer->GetSkyboxPass().SetSunDir(sunLight->direction);
	}


	SoundSystem::GetSoundSystem()->Update(dt);

	UpdateGameCore(dt);

	gameStateManager->Update(dt);
	ProcessState();
	

	debugViewPoint->FinishTime("Update");
	debugViewPoint->MarkTime("Render");
	renderer->Render();
	
	Debug::UpdateRenderables(dt);
	debugViewPoint->FinishTime("Render");
}

void TutorialGame::UpdateGameCore(float dt) {
	Vector2 screenSize = Window::GetWindow()->GetScreenSize();

	if(player){
		Debug::Print(std::string("health: ").append(std::to_string((int)player->GetHealth()->GetHealth())), Vector2(5, 5), Vector4(1, 1, 0, 1));
	}
	if (testingBoss) {
		Debug::Print(std::string("Boss health: ").append(std::to_string((int)testingBoss->GetHealth()->GetHealth())), Vector2(60, 5), Vector4(1, 1, 0, 1));
	}
	if (!inSelectionMode) {
		world->GetMainCamera()->UpdateCamera(dt);
	}
	Vector3 crossPos = CollisionDetection::Unproject(Vector3(screenSize * 0.5f, 0.99f), *world->GetMainCamera());
	Debug::DrawAxisLines(Matrix4::Translation(crossPos), 1.0f);

	world->PreUpdateWorld();

	world->UpdateWorld(dt);
	renderer->Update(dt);
	physics->Update(dt);

	world->PostUpdateWorld();

	if (testingBossBehaviorTree) {
		testingBossBehaviorTree->update();
	}
	
	if (gameLevel->GetShelterTimer() > 20.0f) {
		gameLevel->SetShelterTimer(0.0f);
		UpdateLevel();
	}
}

void TutorialGame::ProcessState() {
	NCL::InputKeyMap& keyMap = NCL::InputKeyMap::instance();
	if (keyMap.GetButton(InputType::Restart)) {
		this->StartLevel();
	}
	if (keyMap.GetButton(InputType::Return)) {
		gameStateManager->SetGameState(GameState::Quit);
	}
}

void TutorialGame::InitCamera() {
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(1000.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));

	world->GetMainCamera()->SetFollow(nullptr);
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

	world->AddGameObject(floor);

	return floor;
}

GameObject* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass) {
	static int id = 0;
	GameObject* sphere = new GameObject( std::string("Sphere").append(std::to_string(id++)));

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(sphere->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("sphere"), nullptr));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* TutorialGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass, bool axisAligned) {
	static int id = 0;
	GameObject* cube = new GameObject( std::string("Cube").append(std::to_string(id++)));

	cube->SetBoundingVolume(axisAligned ? ((CollisionVolume*)new AABBVolume(dimensions)) : ((CollisionVolume*)new OBBVolume(dimensions)));

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(cube->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("cube"), nullptr));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	if (axisAligned) {
		cube->GetPhysicsObject()->InitAxisAlignedInertia();
		cube->GetRenderObject()->SetColour(Vector4(0.8f, 0.8f, 1, 1));
	} else {
		cube->GetPhysicsObject()->InitCubeInertia();
	}
	if (inverseMass == 0) {
		cube->GetPhysicsObject()->SetStatic();
		cube->GetRenderObject()->SetColour(Vector4(1, 0.8f, 0.8f, 1));
	}

	world->AddGameObject(cube);

	return cube;
}

GameObject* TutorialGame::AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass) {
	static int id = 0;
	GameObject* capsule = new GameObject( std::string("Capsule").append(std::to_string(id++)));

	Vector3 capsuleSize = Vector3(radius, halfHeight, radius);
	CapsuleVolume* volume = new CapsuleVolume(halfHeight, radius);
	capsule->SetBoundingVolume((CollisionVolume*)volume);

	capsule->GetTransform()
		.SetScale(capsuleSize)
		.SetPosition(position);

	capsule->SetRenderObject(new RenderObject(capsule->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("capsule"), nullptr));
	capsule->SetPhysicsObject(new PhysicsObject(&capsule->GetTransform(), capsule->GetBoundingVolume()));

	capsule->GetPhysicsObject()->SetInverseMass(inverseMass);
	capsule->GetPhysicsObject()->InitCapsuleInertia();

	world->AddGameObject(capsule);

	return capsule;
}

StateGameObject* TutorialGame::AddStateObjectToWorld(const Vector3& position) {
	static int id = 0;
	StateGameObject* sgo = new StateGameObject(std::string("StateGameObject").append(std::to_string(id++)));
	SphereVolume* volume = new SphereVolume(1.0f);

	sgo->SetBoundingVolume((CollisionVolume*)volume);
	sgo->SetRenderObject(new RenderObject(sgo->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("sphere"), nullptr));
	sgo->SetPhysicsObject(new PhysicsObject(&sgo->GetTransform(), sgo->GetBoundingVolume()));

	sgo->GetTransform().SetPosition(position);

	sgo->GetPhysicsObject()->SetInverseMass(5.0f);
	sgo->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(sgo);

	return sgo;
}

PlayerObject* TutorialGame::AddPlayerToWorld(const Vector3& position, bool cameraFollow) {
	static int id = 0;

	PlayerObject* character = new PlayerObject(id++);
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

	world->AddGameObject(character);

	if (cameraFollow) {
		world->GetMainCamera()->SetFollow(&character->GetTransform());
		character->AttachedCamera();
	}

	return character;
}

Boss* TutorialGame::AddBossToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	Boss* boss = new Boss();

	boss->SetBoundingVolume((CollisionVolume*)new AABBVolume(dimensions));

	boss->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	boss->SetRenderObject(new AnimatedRenderObject(boss->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("boss"), AssetLibrary<MeshMaterial>::GetAsset("boss"), AssetLibrary<MeshAnimation>::GetAsset("WalkForward")));

	boss->GetRenderObject()->SetColour({ 1,1,1,1 });
	boss->SetPhysicsObject(new PhysicsObject(&boss->GetTransform(), boss->GetBoundingVolume()));

	boss->GetPhysicsObject()->SetInverseMass(inverseMass);
	boss->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(boss);

	return boss;
}

void TutorialGame::BuildLevel()
{
	interval = 5.0f;
	gameLevel = new GameLevel{};
	gameLevel->AddRectanglarLevel("BasicLevel.txt", { -100, 0, -70 }, interval);
	world->AddGameObject(gameLevel);
	UpdateLevel();
}

void TutorialGame::UpdateLevel()
{
	for (auto& object : gameLevel->GetGameStuffs())
	{
		if (object->HasDestroyed())
		{
			object->Destroy(false);
			if (object->objectType == ObjectType::Pillar)
			{
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
				world->AddGameObject(pillar);
			}
			if (object->objectType == ObjectType::FenceX)
			{
				Vector3 dimensions{ interval / 4.0f, 0.5f, interval / 5.0f };
				Obstacle* fenceX = new Obstacle{ object, true };
				fenceX->SetBoundingVolume((CollisionVolume*)new AABBVolume(dimensions, CollisionLayer::PaintAble));
				fenceX->GetTransform()
					.SetPosition(object->worldPos + Vector3{ 0,2,0 })
					.SetScale(dimensions * 2);
				//fenceX->SetRenderObject(new RenderObject(&fenceX->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("fenceX"), basicTex, nullptr));		// TODO: change to the right Mesh
				
				PaintRenderObject* render = new PaintRenderObject(fenceX->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("fenceX"), nullptr);
				fenceX->SetRenderObject(render);

				fenceX->SetPhysicsObject(new PhysicsObject(&fenceX->GetTransform(), fenceX->GetBoundingVolume()));
				fenceX->GetPhysicsObject()->SetInverseMass(0);
				fenceX->GetPhysicsObject()->InitCubeInertia();
				world->AddGameObject(fenceX);
			}
			if (object->objectType == ObjectType::FenceY)
			{
				Vector3 dimensions{ interval / 5.0f, 0.5f, interval / 4.0f };
				Obstacle* fenceY = new Obstacle{ object, true };
				fenceY->SetBoundingVolume((CollisionVolume*)new AABBVolume(dimensions, CollisionLayer::PaintAble));
				fenceY->GetTransform()
					.SetPosition(object->worldPos + Vector3{ 0,2,0 })
					.SetScale(dimensions * 2);
				//fenceY->SetRenderObject(new RenderObject(&fenceY->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("fenceY"), basicTex, nullptr));		// TODO: change to the right Mesh
				
				PaintRenderObject* render = new PaintRenderObject(fenceY->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("fenceY"), nullptr);
				fenceY->SetRenderObject(render);
				
				fenceY->SetPhysicsObject(new PhysicsObject(&fenceY->GetTransform(), fenceY->GetBoundingVolume()));
				fenceY->GetPhysicsObject()->SetInverseMass(0);
				fenceY->GetPhysicsObject()->InitCubeInertia();
				world->AddGameObject(fenceY);
			}
			if (object->objectType == ObjectType::Shelter)
			{
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
				world->AddGameObject(shelter);
			}
		}
	}
}

GameObject* TutorialGame::AddTriggerToWorld(const Vector3& position, float size) {
	GameObject* trigger = new GameObject("Trigger");

	SphereVolume* volume = new SphereVolume(size);
	trigger->SetBoundingVolume((CollisionVolume*)volume);
	trigger->GetTransform()
		.SetScale(Vector3(size))
		.SetPosition(position);

	trigger->SetRenderObject(new RenderObject(trigger->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("sphere"), nullptr));
	trigger->GetRenderObject()->SetColour(Vector4(1, 0.5f, 0.5f, 0.5f));

	trigger->SetPhysicsObject(new PhysicsObject(&trigger->GetTransform(), trigger->GetBoundingVolume(), true));
	trigger->GetPhysicsObject()->SetInverseMass(0);
	trigger->GetPhysicsObject()->InitAxisAlignedInertia();
	trigger->GetPhysicsObject()->SetStatic();

	world->AddGameObject(trigger);

	return trigger;
}
