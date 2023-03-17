#ifdef _ORBIS
#include "PS4TutorialGame.h"
#include "PS4Renderer.h"
#include "../CSC8503CoreClasses/RenderObject.h"
#include "../CSC8503CoreClasses/PhysicsObject.h"
#include "../CSC8503CoreClasses/PhysicsSystem.h"
#include "PlayerObject.h"
#include "Boss.h"
#include "InputKeyMap.h"
#include "BulletInstanceManager.h"
#include "../CSC8503CoreClasses/PrefabLibrary.h"

using namespace NCL;

using namespace CSC8503;

TutorialGame::TutorialGame() {

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

	delete basicTex;
}

void NCL::CSC8503::TutorialGame::InitWorld(InitMode mode)
{
	world->ClearAndErase();
	physics->Clear();
	BulletInstanceManager::instance().ObjectIntiation();
	player = AddPlayerToWorld(Vector3(0, 10, 10));
	AddCubeToWorld(Vector3(0, 10, 0), Vector3(1, 1, 1));
	AddSphereToWorld(Vector3(5, 20, 0), 5);
	InitDefaultFloor();

	world->UpdateStaticTree();

	InitCamera();
	SetCameraFollow(player);

	boss = AddBossToWorld({ 0, 5, -20 }, Vector3(4), 2);
	boss->SetTarget(player);
	boss->SetNextTarget(player);
}

void NCL::CSC8503::TutorialGame::UpdateGame(float dt)
{
	world->GetMainCamera()->UpdateCamera(dt);
	player->Update(dt);
	if (timePassed > 30.0f)
		boss->Update(dt);
	timePassed += dt;
	world->UpdateDynamicTree();
	physics->Update(dt);
	renderer->UpdateViewProjectionMatrix((PS4::PS4Camera*)world->GetMainCamera());
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

	basicTex = PS4::PS4Texture::LoadTextureFromFile("doge.gnf");

#ifdef _ORBIS
	PS4::PS4Shader* s = (PS4::PS4Shader*)renderer->LoadShader("/app0/Assets/Shaders/PS4/VertexShader.sb",
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
#endif //_ORBIS