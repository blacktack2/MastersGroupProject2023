#include "TutorialGame.h"

#include "AssetLibrary.h"
#include "PrefabLibrary.h"

#include "PlayerBullet.h"
#include "Bonus.h"
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
#include "TextureLoader.h"

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
	debugViewPoint = &paintHell::debug::DebugViewPoint::Instance();
	
	SoundSystem::Initialize();
	gridManager = &GameGridManager::instance();
	InitialiseAssets();
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
	delete testingBossBehaviorTree;
	testingBossBehaviorTree = nullptr;
}

void TutorialGame::InitWorld() {
	Clear();

	gridManager->AddGameGrid(new GameGrid({ 0,0,0 }, 300, 300, 2));
	BuildLevel();

	InitGameExamples();



	world->UpdateStaticTree();

	selectionObject = nullptr;

	InitCamera();
}

void TutorialGame::UpdateGame(float dt) {
	GameState gameState = gameStateManager->GetGameState();
	keyMap.Update();

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
	if (!inSelectionMode) {
		world->GetMainCamera()->UpdateCamera(dt);
	}
	Vector3 crossPos = CollisionDetection::Unproject(Vector3(screenSize * 0.5f, 0.99f), *world->GetMainCamera());
	Debug::DrawAxisLines(Matrix4::Translation(crossPos), 1.0f);
	if (lockedObject != nullptr) {
		Vector3 objPos = lockedObject->GetTransform().GetGlobalPosition();
		Vector3 camPos = objPos + lockedOffset;

		Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0, 1, 0));

		Matrix4 modelMat = temp.Inverse();

		Quaternion q(modelMat);
		Vector3 angles = q.ToEuler(); //nearly there now!

		world->GetMainCamera()->SetPosition(camPos);
		world->GetMainCamera()->SetPitch(angles.x);
		world->GetMainCamera()->SetYaw(angles.y);
	}

	RayCollision closestCollision;
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::K) && selectionObject) {
		Vector3 rayPos;
		Vector3 rayDir;

		rayDir = selectionObject->GetTransform().GetGlobalOrientation() * Vector3(0, 0, -1);

		rayPos = selectionObject->GetTransform().GetGlobalPosition();

		Ray r = Ray(rayPos, rayDir);

		if (world->Raycast(r, closestCollision, true, selectionObject)) {
			if (objClosest) {
				objClosest->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
			}
			objClosest = (GameObject*)closestCollision.node;

			objClosest->GetRenderObject()->SetColour(Vector4(1, 0, 1, 1));
		}
	}

	world->PreUpdateWorld();

	world->UpdateWorld(dt);
	renderer->Update(dt);
	physics->Update(dt);

	world->PostUpdateWorld();
	if (testingBossBehaviorTree) {
		testingBossBehaviorTree->update();
	}
	
	if (gameLevel->GetShelterTimer() > 20.0f)
	{
		gameLevel->SetShelterTimer(0.0f);
		UpdateLevel();							// rebuild Shelters that has been destroyed
	}
}

void TutorialGame::ProcessState() {
	paintHell::InputKeyMap& keyMap = paintHell::InputKeyMap::instance();
	if (keyMap.GetButton(InputType::Restart)) {
		this->StartLevel();
	}
	if (keyMap.GetButton(InputType::Return)) {
		gameStateManager->SetGameState(GameState::Quit);
	}
}

void TutorialGame::InitialiseAssets() {
	cubeMesh = AssetLibrary::GetMesh("cube");
	sphereMesh  = AssetLibrary::GetMesh("sphere");
	charMesh    = AssetLibrary::GetMesh("goat");
	capsuleMesh = AssetLibrary::GetMesh("capsule");

	basicTex = AssetLibrary::GetTexture("basic");
	healingKitTex = AssetLibrary::GetTexture("healingKitTex");
	pillarTex = AssetLibrary::GetTexture("pillarTex");

	InitaliseAnimationAssets();

	//old stuff
	enemyMesh = AssetLibrary::GetMesh("cube");
	npcMesh = AssetLibrary::GetMesh("cube");
	bonusMesh = AssetLibrary::GetMesh("cube");

	InitialisePrefabs();
}

void TutorialGame::InitaliseAnimationAssets() {
	maleguardMaterial = AssetLibrary::GetMaterial("boss");
	maleguardMesh = AssetLibrary::GetMesh("boss");
	maleguardAnim = AssetLibrary::GetAnimation("WalkForward");

	//for (int i = 0; i < maleguardMesh->GetSubMeshCount(); ++i) {
	//	const MeshMaterialEntry* matEntry = maleguardMaterial->GetMaterialForLayer(i);

	//	const string* filename = nullptr;

	//	matEntry->GetEntry("Diffuse", &filename);
	//	OGLTexture* tex = nullptr;

	//	if (filename) {
	//		string path = Assets::TEXTUREDIR + *filename;
	//		stbi_set_flip_vertically_on_load(true);
	//		tex = static_cast<OGLTexture*>(renderer->LoadTexture(path));
	//		stbi_set_flip_vertically_on_load(false);
	//		tex->SetEdgeRepeat();
	//	}
	//	maleguardMatTextures.emplace_back(tex);
	//}
}

void TutorialGame::InitialisePrefabs() {
	float bulletRadius = 0.2f;
	GameObject* bulletPrefab;
	bulletPrefab = new PlayerBullet();

	bulletPrefab->SetBoundingVolume((CollisionVolume*) new SphereVolume(bulletRadius, CollisionLayer::PlayerProj));
	bulletPrefab->GetTransform().SetScale(Vector3(bulletRadius));

	bulletPrefab->SetRenderObject(new RenderObject(&bulletPrefab->GetTransform(), sphereMesh, nullptr));
	bulletPrefab->SetPhysicsObject(new PhysicsObject(&bulletPrefab->GetTransform(), bulletPrefab->GetBoundingVolume(), true));
	bulletPrefab->GetRenderObject()->SetColour(Vector4(1, 0.5f, 0.8f, 1.0f));

	bulletPrefab->GetPhysicsObject()->SetInverseMass(1.0f);
	bulletPrefab->GetPhysicsObject()->SetGravWeight(1.0f);
	bulletPrefab->GetPhysicsObject()->InitCapsuleInertia();

	PrefabLibrary::AddPrefab("bullet", bulletPrefab);

	bulletRadius = 0.75f;

	bulletPrefab = new BossBullet();

	bulletPrefab->SetBoundingVolume((CollisionVolume*) new SphereVolume(bulletRadius, CollisionLayer::EnemyProj));
	bulletPrefab->GetTransform().SetScale(Vector3(bulletRadius));

	bulletPrefab->SetRenderObject(new RenderObject(&bulletPrefab->GetTransform(), sphereMesh, nullptr));
	bulletPrefab->SetPhysicsObject(new PhysicsObject(&bulletPrefab->GetTransform(), bulletPrefab->GetBoundingVolume(), true));

	bulletPrefab->GetRenderObject()->SetColour(Vector4(0.2f, 1.0f, 0.5f, 1.0f));

	bulletPrefab->GetPhysicsObject()->SetInverseMass(1.0f);
	bulletPrefab->GetPhysicsObject()->SetGravWeight(1.0f);
	bulletPrefab->GetPhysicsObject()->InitCapsuleInertia();

	PrefabLibrary::AddPrefab("bossBullet", bulletPrefab);
}

void TutorialGame::InitCamera() {
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(1000.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
	world->GetMainCamera()->SetFollow(nullptr);
	lockedObject = nullptr;
}

void TutorialGame::InitGameExamples()/*for audio testing*/ {
	
	InitDefaultFloor();
	SoundSystem::GetSoundSystem()->SetMasterVolume(1.0f);
}

void TutorialGame::InitMazeWorld(int numRows, int numCols, float size) {
	mazes = new Maze[1]{ Maze(*world, size, 100.0f, numRows, numCols, Vector3(0, 0, 0)) };

	NavigationGrid& nav = mazes[0].GetNavGrid();

	Vector3 position;
	while (!mazes[0].ValidPoint(position = Vector3(((rand() % 400) - 200), 5, (rand() % 400) - 200))) {}
	//AddEnemyToWorld(position, nav);

	for (int i = 0; i < 20; i++) {
		while (!mazes[0].ValidPoint(position = Vector3(((rand() % 400) - 200), 4, (rand() % 400) - 200))) {}
		AddBonusToWorld(position);
	}

	for (int i = 0; i < 50; i++) {
		while (!mazes[0].ValidPoint(position = Vector3(((rand() % 400) - 200), 5, (rand() % 400) - 200))) {}
		AddNPCToWorld(position);
	}

	for (int i = 0; i < 100; i++) {
		while (!mazes[0].ValidPoint(position = Vector3(((rand() % 400) - 200), 1.5f + rand() % 30, (rand() % 400) - 200))) {}
		switch (rand() % 5) {
			case 0: AddCubeToWorld(position, Vector3(2), 20.0f, true); break;
			case 1: AddCubeToWorld(position, Vector3(1), 10.0f, false); break;
			case 2: AddSphereToWorld(position, 1.5f); break;
			case 3: AddCapsuleToWorld(position, 1.0f, 1.0f); break;
			case 4: AddCubeToWorld(position, Vector3(1.5f), 0.0f, true); break;
		}
	}
}

void TutorialGame::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing) {
	float sphereRadius = 1.0f;
	Vector3 cubeDims = Vector3(1, 1, 1);
	float capsuleHeight = 1.0f;
	float capsuleRadius = 1.0f;
	Vector3 offset = Vector3(numRows * rowSpacing, 0, numCols * colSpacing) * -0.5;

	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = offset + Vector3(x * colSpacing, 10.0f, z * rowSpacing);

			switch (rand() % 5) {
			case 0: AddCubeToWorld(position, cubeDims, 10.0f, true); break;
			case 1: AddCubeToWorld(position, cubeDims, 10.0f, false); break;
			case 2: AddSphereToWorld(position, sphereRadius); break;
			case 3: AddCapsuleToWorld(position, capsuleHeight, capsuleRadius); break;
			case 4: AddCubeToWorld(position, cubeDims, 0.0f, true); break;
			}
		}
	}
}

void TutorialGame::InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims, bool axisAligned) {
	Vector3 offset = Vector3(numRows * rowSpacing, 0, numCols * colSpacing) * -0.5;
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = offset + Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddCubeToWorld(position, cubeDims, 1.0f, axisAligned);
		}
	}
}

void TutorialGame::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius) {
	Vector3 offset = Vector3(numRows * rowSpacing, 0, numCols * colSpacing) * -0.5;
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = offset + Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddSphereToWorld(position, radius, 1.0f);
		}
	}
}

void TutorialGame::InitBridgeConstraintTestWorld(int numLinks, float cubeDistance, float maxDistance, bool isOrientation) {
	Vector3 size = Vector3(8);

	float invMass = 5.0f;

	Vector3 startPos = Vector3(-maxDistance * numLinks / 2, 100, 0);

	GameObject* start = isOrientation ? AddSphereToWorld(startPos, size.x, 0) : AddCubeToWorld(startPos, size, 0);
	GameObject* previous = start;

	for (int i = 1; i <= numLinks; i++) {
		GameObject* block = isOrientation ? AddSphereToWorld(startPos + Vector3(i * cubeDistance, 0, invMass), size.x, invMass) : AddCubeToWorld(startPos + Vector3(i * cubeDistance, 0, 0), size, invMass);
		world->AddConstraint(new PositionConstraint(previous, block, maxDistance, maxDistance));
		if (isOrientation)
			world->AddConstraint(new OrientationConstraint(previous, block));

		previous = block;
	}

	GameObject* end = isOrientation ? AddSphereToWorld(startPos + Vector3((numLinks + 1) * cubeDistance, 0, 0), size.x, 0) : AddCubeToWorld(startPos + Vector3((numLinks + 1) * cubeDistance, 0, 0), size, 0);
	world->AddConstraint(new PositionConstraint(previous, end, maxDistance, maxDistance));
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
	
	PaintRenderObject* render = new PaintRenderObject(&floor->GetTransform(), cubeMesh, nullptr);
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

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, nullptr));
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

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, nullptr));
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

	capsule->SetRenderObject(new RenderObject(&capsule->GetTransform(), capsuleMesh, nullptr));
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
	sgo->SetRenderObject(new RenderObject(&sgo->GetTransform(), sphereMesh, nullptr));
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

	character->SetRenderObject(new RenderObject(&character->GetTransform(), charMesh, nullptr));
	
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

EnemyObject* TutorialGame::AddEnemyToWorld(const Vector3& position, NavigationMap& navMap) {
	EnemyObject* enemy = new EnemyObject(*player, navMap);
	SphereVolume* volume = new SphereVolume(1.0f, CollisionLayer::Enemy);

	enemy->SetBoundingVolume((CollisionVolume*)volume);

	enemy->GetTransform()
		.SetScale(Vector3(1.0f))
		.SetPosition(position);

	enemy->SetRenderObject(new RenderObject(&enemy->GetTransform(), enemyMesh, nullptr));
	enemy->SetPhysicsObject(new PhysicsObject(&enemy->GetTransform(), enemy->GetBoundingVolume()));

	enemy->GetRenderObject()->SetColour(Vector4(1, 0.9f, 0.8f, 1));

	enemy->GetPhysicsObject()->SetInverseMass(1);
	enemy->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(enemy);

	return enemy;
}

Boss* TutorialGame::AddBossToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	Boss* boss = new Boss();

	boss->SetBoundingVolume((CollisionVolume*)new AABBVolume(dimensions));

	boss->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	boss->SetRenderObject(new AnimatedRenderObject(&boss->GetTransform(), AssetLibrary::GetMesh("boss"), AssetLibrary::GetMaterial("boss"), AssetLibrary::GetAnimation("WalkForward")));

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
		if (object.HasDestroyed())
		{
			object.Destroy(false);
			if (object.objectType == ObjectType::Pillar)
			{
				Vector3 dimensions{ interval / 2.0f, 10, interval / 2.0f };
				Obstacle* pillar = new Obstacle{ &object, true };
				pillar->SetBoundingVolume((CollisionVolume*)new AABBVolume(dimensions * Vector3{ 1.3,2,1.3 }, CollisionLayer::PaintAble));

				pillar->GetTransform()
					.SetPosition(object.worldPos + Vector3{ 0,20,0 })
					.SetScale(dimensions * 2);
				//pillar->SetRenderObject(new RenderObject(&pillar->GetTransform(), AssetLibrary::GetMesh("pillar"), healingKitTex, nullptr));
				
				PaintRenderObject* render = new PaintRenderObject(&pillar->GetTransform(), AssetLibrary::GetMesh("pillar"), AssetLibrary::GetMaterial("pillar"));
				pillar->SetRenderObject(render);
				
				pillar->SetPhysicsObject(new PhysicsObject(&pillar->GetTransform(), pillar->GetBoundingVolume()));
				pillar->GetPhysicsObject()->SetInverseMass(0);
				pillar->GetPhysicsObject()->InitCubeInertia();
				world->AddGameObject(pillar);
			}
			if (object.objectType == ObjectType::FenceX)
			{
				Vector3 dimensions{ interval / 4.0f, 0.5f, interval / 5.0f };
				Obstacle* fenceX = new Obstacle{ &object, true };
				fenceX->SetBoundingVolume((CollisionVolume*)new AABBVolume(dimensions, CollisionLayer::PaintAble));
				fenceX->GetTransform()
					.SetPosition(object.worldPos + Vector3{ 0,2,0 })
					.SetScale(dimensions * 2);
				//fenceX->SetRenderObject(new RenderObject(&fenceX->GetTransform(), AssetLibrary::GetMesh("fenceX"), basicTex, nullptr));		// TODO: change to the right Mesh
				
				PaintRenderObject* render = new PaintRenderObject(&fenceX->GetTransform(), AssetLibrary::GetMesh("fenceX"), nullptr);
				fenceX->SetRenderObject(render);

				fenceX->SetPhysicsObject(new PhysicsObject(&fenceX->GetTransform(), fenceX->GetBoundingVolume()));
				fenceX->GetPhysicsObject()->SetInverseMass(0);
				fenceX->GetPhysicsObject()->InitCubeInertia();
				world->AddGameObject(fenceX);
			}
			if (object.objectType == ObjectType::FenceY)
			{
				Vector3 dimensions{ interval / 5.0f, 0.5f, interval / 4.0f };
				Obstacle* fenceY = new Obstacle{ &object, true };
				fenceY->SetBoundingVolume((CollisionVolume*)new AABBVolume(dimensions, CollisionLayer::PaintAble));
				fenceY->GetTransform()
					.SetPosition(object.worldPos + Vector3{ 0,2,0 })
					.SetScale(dimensions * 2);
				//fenceY->SetRenderObject(new RenderObject(&fenceY->GetTransform(), AssetLibrary::GetMesh("fenceY"), basicTex, nullptr));		// TODO: change to the right Mesh
				
				PaintRenderObject* render = new PaintRenderObject(&fenceY->GetTransform(), AssetLibrary::GetMesh("fenceY"), nullptr);
				fenceY->SetRenderObject(render);
				
				fenceY->SetPhysicsObject(new PhysicsObject(&fenceY->GetTransform(), fenceY->GetBoundingVolume()));
				fenceY->GetPhysicsObject()->SetInverseMass(0);
				fenceY->GetPhysicsObject()->InitCubeInertia();
				world->AddGameObject(fenceY);
			}
			if (object.objectType == ObjectType::Shelter)
			{
				Vector3 dimensions{ interval / 5.0f, 2.0f, interval / 2.0f };
				Obstacle* shelter = new Obstacle{ &object, false };
				shelter->SetBoundingVolume((CollisionVolume*)new AABBVolume(dimensions, CollisionLayer::PaintAble));
				shelter->GetTransform()
					.SetPosition(object.worldPos + Vector3{ 0,2.2,0 })
					.SetScale(dimensions);
				//shelter->SetRenderObject(new RenderObject(&shelter->GetTransform(), AssetLibrary::GetMesh("shelter"), basicTex, nullptr));

				PaintRenderObject* render = new PaintRenderObject(&shelter->GetTransform(), AssetLibrary::GetMesh("shelter"), nullptr);
				shelter->SetRenderObject(render);

				shelter->SetPhysicsObject(new PhysicsObject(&shelter->GetTransform(), shelter->GetBoundingVolume()));
				shelter->GetPhysicsObject()->SetInverseMass(0);
				shelter->GetPhysicsObject()->InitCubeInertia();
				world->AddGameObject(shelter);
			}
			/*
			if (object.objectType == ObjectType::Wall)
			{
				Vector3 dimensions{ interval / 2.0f, 30.0f, interval / 2.0f };
				Obstacle* wall = new Obstacle{ &object, true };
				wall->SetBoundingVolume((CollisionVolume*)new AABBVolume(dimensions, CollisionLayer::PaintAble));
				wall->GetTransform()
					.SetPosition(object.worldPos)
					.SetScale(dimensions * 2);
				//wall->SetRenderObject(new RenderObject(&wall->GetTransform(), AssetLibrary::GetMesh("wall"), basicTex, nullptr));

				PaintRenderObject* render = new PaintRenderObject(&wall->GetTransform(), AssetLibrary::GetMesh("wall"), basicTex);
				wall->SetRenderObject(render);

				wall->SetPhysicsObject(new PhysicsObject(&wall->GetTransform(), wall->GetBoundingVolume()));
				wall->GetPhysicsObject()->SetInverseMass(0);
				wall->GetPhysicsObject()->InitCubeInertia();
				world->AddGameObject(wall);
			}
			*/
		}
	}
}

NPCObject* TutorialGame::AddNPCToWorld(const Vector3& position) {
	NPCObject* npc = new NPCObject();
	CapsuleVolume* volume = new CapsuleVolume(1.5f, 1.0f);

	npc->SetBoundingVolume((CollisionVolume*)volume);

	npc->GetTransform()
		.SetScale(Vector3(2.0f))
		.SetPosition(position);

	npc->SetRenderObject(new RenderObject(&npc->GetTransform(), npcMesh, nullptr));
	npc->SetPhysicsObject(new PhysicsObject(&npc->GetTransform(), npc->GetBoundingVolume()));

	npc->GetRenderObject()->SetColour(Vector4(1, 1, 0.8f, 1));

	npc->GetPhysicsObject()->SetInverseMass(1);
	npc->GetPhysicsObject()->InitCapsuleInertia();

	world->AddGameObject(npc);

	return npc;
}

GameObject* TutorialGame::AddBonusToWorld(const Vector3& position) {
	GameObject* bonus = new BonusObject();
	SphereVolume* volume = new SphereVolume(1.5f);

	bonus->SetBoundingVolume((CollisionVolume*)volume);

	bonus->GetTransform()
		.SetScale(Vector3(1))
		.SetPosition(position);

	bonus->SetRenderObject(new RenderObject(&bonus->GetTransform(), bonusMesh, nullptr));
	bonus->SetPhysicsObject(new PhysicsObject(&bonus->GetTransform(), bonus->GetBoundingVolume(), true));

	bonus->GetRenderObject()->SetColour(Vector4(1, 0.2f, 0.2f, 1));

	bonus->GetPhysicsObject()->SetInverseMass(0.0f);
	bonus->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(bonus);

	return bonus;
}

GameObject* TutorialGame::AddTriggerToWorld(const Vector3& position, float size) {
	GameObject* trigger = new GameObject("Trigger");

	SphereVolume* volume = new SphereVolume(size);
	trigger->SetBoundingVolume((CollisionVolume*)volume);
	trigger->GetTransform()
		.SetScale(Vector3(size))
		.SetPosition(position);

	trigger->SetRenderObject(new RenderObject(&trigger->GetTransform(), sphereMesh, nullptr));
	trigger->GetRenderObject()->SetColour(Vector4(1, 0.5f, 0.5f, 0.5f));

	trigger->SetPhysicsObject(new PhysicsObject(&trigger->GetTransform(), trigger->GetBoundingVolume(), true));
	trigger->GetPhysicsObject()->SetInverseMass(0);
	trigger->GetPhysicsObject()->InitAxisAlignedInertia();
	trigger->GetPhysicsObject()->SetStatic();

	world->AddGameObject(trigger);

	return trigger;
}

bool TutorialGame::SelectObject() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q)) {
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}
	if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT)) {
		if (selectionObject) {
			selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
			selectionObject = nullptr;
		}

		Ray ray = CollisionDetection::BuildRayFromCamera(*world->GetMainCamera());

		RayCollision closestCollision;
		if (world->Raycast(ray, closestCollision, true)) {
			selectionObject = (GameObject*)closestCollision.node;

			selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));

			Debug::DrawLine(ray.GetPosition(), closestCollision.collidedAt, Vector4(0, 0, 1, 1), 5.0f);

			Ray objectRay = Ray(selectionObject->GetTransform().GetGlobalPosition(), selectionObject->GetTransform().GetGlobalOrientation() * Vector3(0, 0, 1), selectionObject->GetBoundingVolume()->layer);
			return true;
		}
		else {
			return false;
		}
	}
	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::L)) {
		if (selectionObject) {
			if (lockedObject == selectionObject) {
				lockedObject = nullptr;
			}
			else {
				lockedObject = selectionObject;
			}
		}
	}
	return false;
}

void TutorialGame::MoveSelectedObject() {
	Debug::Print("Click Force:" + std::to_string(forceMagnitude), Vector2(5, 90));
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 100.0f;

	if (!selectionObject) {
		return;//we haven't selected anything!
	}
	//Push the selected object!
	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::RIGHT)) {
		Ray ray = CollisionDetection::BuildRayFromCamera(*world->GetMainCamera());

		RayCollision closestCollision;
		if (world->Raycast(ray, closestCollision, true)) {
			if (closestCollision.node == selectionObject) {
				selectionObject->GetPhysicsObject()->AddForceAtPosition(ray.GetDirection() * forceMagnitude, closestCollision.collidedAt);
			}
		}
	}
}

void TutorialGame::DebugObjectMovement() {
	//If we've selected an object, we can manipulate it with some key presses
	if (selectionObject) {
		//Twist the selected object!
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(-10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM7)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM8)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, -10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, 10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM5)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
		}
	}
}

void TutorialGame::LockedObjectMovement() {
	Matrix4 view		= world->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld	= view.Inverse();

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis.Normalise();


	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
		selectionObject->GetPhysicsObject()->AddForce(fwdAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
		selectionObject->GetPhysicsObject()->AddForce(-fwdAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NEXT)) {
		selectionObject->GetPhysicsObject()->AddForce(Vector3(0,-10,0));
	}
}
