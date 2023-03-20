#include "TutorialGame.h"
#include "PS4Renderer.h"
#include "../Common/RenderObject.h"
#include "../Common/PhysicsObject.h"
#include "../Common/PhysicsSystem.h"
#include "PlayerObject.h"
#include "Boss.h"
#include "InputKeyMap.h"
#include "BulletInstanceManager.h"
#include "../Common/PrefabLibrary.h"
#include "GameGridManager.h"
#include "GameStateManager.h"
#include "../Common/Debug.h"
#include "Obstacle.h"
using namespace NCL;

using namespace CSC8503;

TutorialGame::TutorialGame():gridManager(GameGridManager::instance()),
gameStateManager(GameStateManager::instance()) {

	world = &GameWorld::instance();
	
	/*sunLight = world->AddLight(new Light({ 0, 0, 0, 0 }, { 1, 1, 1, 1 }, 0, { 0.9f, 0.4f, 0.1f }));
	world->AddLight(new Light({ 0.0f, 5.0f, -10.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }, 10.0f));*/
#ifdef USEVULKAN
	renderer = new GameTechVulkanRenderer(*world);
#else 
	renderer = new PS4::PS4Renderer(*world);
#endif

	physics = new PhysicsSystem(*world);

	forceMagnitude = 10.0f;
	inSelectionMode = false;

	

	InitialiseAssets();
	float bulletRadius = 0.2f;
	std::unique_ptr<GameObject> bulletPrefab = std::make_unique<PlayerBullet>();

	bulletPrefab->SetBoundingVolume((CollisionVolume*) new SphereVolume(bulletRadius, CollisionLayer::PlayerProj));
	bulletPrefab->GetTransform().SetScale(Vector3(bulletRadius));

	bulletPrefab->SetRenderObject(new RenderObject(&bulletPrefab->GetTransform(), sphereMesh, basicTex));
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

	bulletPrefab->SetRenderObject(new RenderObject(&bulletPrefab->GetTransform(), sphereMesh, basicTex));
	bulletPrefab->SetPhysicsObject(new PhysicsObject(&bulletPrefab->GetTransform(), bulletPrefab->GetBoundingVolume(), true));

	bulletPrefab->GetRenderObject()->SetColour(Vector4(0.2f, 1.0f, 0.5f, 1.0f));

	bulletPrefab->GetPhysicsObject()->SetInverseMass(1.0f);
	bulletPrefab->GetPhysicsObject()->SetGravWeight(1.0f);
	bulletPrefab->GetPhysicsObject()->InitCapsuleInertia();

	PrefabLibrary::AddPrefab("bossBullet", std::move(bulletPrefab));
	InitWorld();


}

NCL::CSC8503::TutorialGame::~TutorialGame()
{
	delete cubeMesh;
	delete sphereMesh;
	delete charMesh;
	delete npcMesh;
	delete capsuleMesh;
	delete enemyMesh;
	delete bonusMesh;
	delete pillarMesh;
	delete fenceMesh;

	delete basicTex;
}

void NCL::CSC8503::TutorialGame::InitWorld(InitMode mode)
{
	world->ClearAndErase();
	physics->Clear();
	gridManager.AddGameGrid(new GameGrid(Vector3(0.0f), 400, 400, 1));
	BuildLevel();

	BulletInstanceManager::instance().ObjectIntiation();
	player = AddPlayerToWorld(Vector3(0,10,10));
	
	//AddSphereToWorld(Vector3(5, 20, 0), 5);
	InitDefaultFloor();
	
	world->UpdateStaticTree();

	InitCamera();
	SetCameraFollow(player);
	boss = AddBossToWorld({ 0, 200, 0 }, Vector3( 4 ), 2);
	boss->SetTarget(player);
	boss->SetNextTarget(player);
}

void NCL::CSC8503::TutorialGame::UpdateGame(float dt)
{
	gridManager.Update(dt);
	world->PreUpdateWorld();
	world->UpdateWorld(dt);
	
	Debug::Print("Health:"+std::to_string(player->GetHealth()->GetHealth()), Vector2(5, 10));
	Debug::Print("Boss Health:"+std::to_string(boss->GetHealth()->GetHealth()), Vector2(50, 10));
	
	/*player->Update(dt);
	if(timePassed>30.0f)
		boss->Update(dt);*/
	timePassed += dt;
	
	physics->Update(dt);

	//if (gameLevel->GetShelterTimer() > 20.0f) {
	//	gameLevel->SetShelterTimer(0.0f);
	//	UpdateLevel();
	//}

	world->GetMainCamera()->UpdateCamera(dt);
	gameStateManager.Update(dt);
	ProcessState();

	renderer->UpdateViewProjectionMatrix((PS4::PS4Camera*)world->GetMainCamera());
	Debug::UpdateRenderables(dt);
	renderer->BuildObjectList();
	renderer->Render();
	//gameStateManager.Update(dt);
		
}

void NCL::CSC8503::TutorialGame::InitialiseAssets()
{
	cubeMesh = renderer->LoadMesh("cube.msh");
	sphereMesh = renderer->LoadMesh("sphere.msh");
	charMesh = renderer->LoadMesh("goat.msh");
	enemyMesh = renderer->LoadMesh("goose.msh");
	npcMesh = renderer->LoadMesh("Keeper.msh");
	bonusMesh = renderer->LoadMesh("Sphere.msh");
	capsuleMesh = renderer->LoadMesh("capsule.msh");
	fenceMesh = renderer->LoadMesh("Building/newFence.msh");
	pillarMesh= renderer->LoadMesh("pillarCube.msh");

	basicTex = PS4::PS4Texture::LoadTextureFromFile("doge.gnf");

#ifdef _ORBIS
	PS4::PS4Shader* s= (PS4::PS4Shader*)renderer->LoadShader("/app0/Assets/Shaders/PS4/VertexShader.sb",
		"/app0/Assets/Shaders/PS4/PixelShader.sb");
	renderer->SetDefaultShader(s);
#endif
}

void NCL::CSC8503::TutorialGame::InitCamera()
{
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(1000.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
	
	lockedObject = nullptr;
}

void NCL::CSC8503::TutorialGame::InitDefaultFloor()
{
	floor = AddFloorToWorld(Vector3(0, -2, 0));
}

GameObject* NCL::CSC8503::TutorialGame::AddFloorToWorld(const NCL::Maths::Vector3& position)
{
	GameObject* floor = new GameObject("Floor");

	Vector3 floorSize = Vector3(200, 2, 200);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position);

	RenderObject* render = new RenderObject(&floor->GetTransform(), cubeMesh, basicTex);
	floor->SetRenderObject(render);

	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetStatic();
	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(floor);

	return floor;
}

GameObject* NCL::CSC8503::TutorialGame::AddSphereToWorld(const Maths::Vector3& position, float radius, float inverseMass)
{
	static int id = 0;
	GameObject* sphere = new GameObject(std::string("Sphere").append(std::to_string(id++)));

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, nullptr));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* NCL::CSC8503::TutorialGame::AddCubeToWorld(const Maths::Vector3& position, Maths::Vector3 dimensions, float inverseMass, bool axisAligned)
{
	static int id = 0;
	GameObject* cube = new GameObject(std::string("Cube").append(std::to_string(id++)));

	cube->SetBoundingVolume(axisAligned ? ((CollisionVolume*)new AABBVolume(dimensions)) : ((CollisionVolume*)new OBBVolume(dimensions)));

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, nullptr));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	if (axisAligned) {
		cube->GetPhysicsObject()->InitAxisAlignedInertia();
		cube->GetRenderObject()->SetColour(Vector4(0.8f, 0.8f, 1, 1));
	}
	else {
		cube->GetPhysicsObject()->InitCubeInertia();
	}
	if (inverseMass == 0) {
		cube->GetPhysicsObject()->SetStatic();
		cube->GetRenderObject()->SetColour(Vector4(1, 0.8f, 0.8f, 1));
	}

	world->AddGameObject(cube);

	return cube;
}

Boss* TutorialGame::AddBossToWorld(const Maths::Vector3& position, Maths::Vector3 dimensions, float inverseMass) {
	Boss* boss = new Boss();

	boss->SetBoundingVolume((CollisionVolume*)new AABBVolume(dimensions));

	boss->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions);

	boss->SetRenderObject(new RenderObject(&boss->GetTransform(), enemyMesh, basicTex));

	boss->GetRenderObject()->SetColour({ 1, 1, 1, 1 });
	boss->SetPhysicsObject(new PhysicsObject(&boss->GetTransform(), boss->GetBoundingVolume()));

	boss->GetPhysicsObject()->SetInverseMass(inverseMass);
	boss->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(boss);

	return boss;
}

void NCL::CSC8503::TutorialGame::BuildLevel()
{
	interval = 5.0f;

	gameLevel = new GameLevel();
	gameLevel->AddRectanglarLevel("BasicLevel.txt", { -200, 0, -200 }, interval);
	world->AddGameObject(gameLevel);

	UpdateLevel();
}

void NCL::CSC8503::TutorialGame::UpdateLevel()
{
	for (auto& object : gameLevel->GetGameStuffs()) {
		if (object->HasDestroyed()) {
			object->Destroy(false);
			if (object->objectType == ObjectType::Pillar) {
				Vector3 dimensions{ interval / 2.0f, 15, interval / 2.0f };
				Obstacle* pillar = new Obstacle{ object, true };

				pillar->SetBoundingVolume((CollisionVolume*)new AABBVolume(dimensions * Vector3{ 1.3f, 2.0f, 1.3f }, CollisionLayer::PaintAble));

				pillar->GetTransform()
					.SetPosition(object->worldPos + Vector3{ 0,15,0 })
					.SetScale(dimensions * 2);
				//pillar->SetRenderObject(new RenderObject(&pillar->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("pillar"), healingKitTex, nullptr));

				RenderObject* render = new RenderObject(&pillar->GetTransform(),pillarMesh, basicTex);
				pillar->SetRenderObject(render);

				pillar->SetPhysicsObject(new PhysicsObject(&pillar->GetTransform(), pillar->GetBoundingVolume()));
				pillar->GetPhysicsObject()->SetInverseMass(0);
				pillar->GetPhysicsObject()->InitCubeInertia();
				world->AddGameObject(pillar);
			}
			if (object->objectType == ObjectType::FenceX) {
				Vector3 dimensions{ interval / 2.0f, 3.0f, interval / 5.0f };
				Obstacle* fenceX = new Obstacle{ object, true };
				fenceX->SetBoundingVolume((CollisionVolume*)new AABBVolume(dimensions * 2, CollisionLayer::PaintAble));
				fenceX->GetTransform()
					.SetPosition(object->worldPos + Vector3{ 0,3,0 })
					.SetScale(dimensions * 2);
				//fenceX->SetRenderObject(new RenderObject(&fenceX->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("fenceX"), basicTex, nullptr));		// TODO: change to the right Mesh

				RenderObject* render = new RenderObject(&fenceX->GetTransform(), fenceMesh, basicTex);
				fenceX->SetRenderObject(render);

				fenceX->SetPhysicsObject(new PhysicsObject(&fenceX->GetTransform(), fenceX->GetBoundingVolume()));
				fenceX->GetPhysicsObject()->SetInverseMass(0);
				fenceX->GetPhysicsObject()->InitCubeInertia();
				world->AddGameObject(fenceX);
			}
			if (object->objectType == ObjectType::FenceY) {
				Vector3 dimensions{ interval / 5.0f, 3.0f, interval / 2.0f };
				Obstacle* fenceY = new Obstacle{ object, true };
				fenceY->SetBoundingVolume((CollisionVolume*)new AABBVolume(dimensions * 2, CollisionLayer::PaintAble));
				fenceY->GetTransform()
					.SetPosition(object->worldPos + Vector3{ 0,3,0 })
					.SetScale(dimensions * 2);
				//fenceY->SetRenderObject(new RenderObject(&fenceY->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("fenceY"), basicTex, nullptr));		// TODO: change to the right Mesh

				RenderObject* render = new RenderObject(&fenceY->GetTransform(), fenceMesh, basicTex);
				fenceY->SetRenderObject(render);

				fenceY->SetPhysicsObject(new PhysicsObject(&fenceY->GetTransform(), fenceY->GetBoundingVolume()));
				fenceY->GetPhysicsObject()->SetInverseMass(0);
				fenceY->GetPhysicsObject()->InitCubeInertia();
				world->AddGameObject(fenceY);
			}
			if (object->objectType == ObjectType::Shelter) {
				Vector3 dimensions{ interval / 1.0f, 5.0f, interval / 1.0f };
				Obstacle* shelter = new Obstacle{ object, false };
				shelter->SetBoundingVolume((CollisionVolume*)new AABBVolume(dimensions * Vector3{ 0.5f,0.8f,1.0f }, CollisionLayer::PaintAble));
				shelter->GetTransform()
					.SetPosition(object->worldPos + Vector3{ 0.0f,2.7f, 0.0f })
					.SetScale(dimensions);
				//shelter->SetRenderObject(new RenderObject(&shelter->GetTransform(), AssetLibrary<MeshGeometry>::GetAsset("shelter"), basicTex, nullptr));

				RenderObject* render = new RenderObject(&shelter->GetTransform(),cubeMesh, basicTex);
				shelter->SetRenderObject(render);

				shelter->SetPhysicsObject(new PhysicsObject(&shelter->GetTransform(), shelter->GetBoundingVolume()));
				shelter->GetPhysicsObject()->SetInverseMass(0);
				shelter->GetPhysicsObject()->InitCubeInertia();
				world->AddGameObject(shelter);
			}
		}
	}
}

void NCL::CSC8503::TutorialGame::ProcessState()
{
	if (player->GetHealth()->GetHealth() <= 0) {
		gameStateManager.SetGameState(GameState::Lose);
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
		this->InitWorld();
	}
	if (keyMap.GetButton(InputType::Return)) {
		gameStateManager.SetGameState(GameState::Quit);
	}
}


GameObject* NCL::CSC8503::TutorialGame::AddCapsuleToWorld(const Maths::Vector3& position, float halfHeight, float radius, float inverseMass)
{
	static int id = 0;
	GameObject* capsule = new GameObject(std::string("Capsule").append(std::to_string(id++)));

	Vector3 capsuleSize = Vector3(radius, halfHeight, radius);
	CapsuleVolume* volume = new CapsuleVolume(halfHeight, radius);
	capsule->SetBoundingVolume((CollisionVolume*)volume);

	capsule->GetTransform()
		.SetScale(capsuleSize)
		.SetPosition(position);

	capsule->SetRenderObject(new RenderObject(&capsule->GetTransform(), capsuleMesh, basicTex, nullptr));
	capsule->SetPhysicsObject(new PhysicsObject(&capsule->GetTransform(), capsule->GetBoundingVolume()));

	capsule->GetPhysicsObject()->SetInverseMass(inverseMass);
	capsule->GetPhysicsObject()->InitCapsuleInertia();

	world->AddGameObject(capsule);

	return capsule;
}

PlayerObject* NCL::CSC8503::TutorialGame::AddPlayerToWorld(const Maths::Vector3& position, bool cameraFollow)
{
	static int id = 0;

	PlayerObject* character = new PlayerObject(0);
	SphereVolume* volume = new SphereVolume(1.0f, CollisionLayer::Player);

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(1, 1, 1))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), charMesh, basicTex));

	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetRenderObject()->SetColour(Vector4(1, 0.9f, 0.8f, 1));

	character->GetPhysicsObject()->SetInverseMass(1);
	character->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(character);

	return character;
}

void TutorialGame::SetCameraFollow(PlayerObject* p)
{
	int id = p->GetPlayerID();
	world->SetMainCamera(id);
	world->GetMainCamera()->SetFollow(&(p->GetTransform()));
}