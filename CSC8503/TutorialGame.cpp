#include "AssetLibrary.h"
#include "Bullet.h"
#include "Bonus.h"
#include "Debug.h"
#include "GameWorld.h"
#include "Maze.h"
#include "OrientationConstraint.h"
#include "PhysicsObject.h"
#include "PlayerObject.h"
#include "PositionConstraint.h"
#include "RenderObject.h"
#include "StateGameObject.h"
#include "TextureLoader.h"
#include "TutorialGame.h"

using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame(){
	world = &GameWorld::instance();
#ifdef USEVULKAN
	renderer	= new GameTechVulkanRenderer(*world);
#else 
	renderer = new GameTechRenderer(*world);
#endif

	physics		= new PhysicsSystem(*world);

	forceMagnitude	= 10.0f;
	inSelectionMode = false;
	debugViewPoint = &paintHell::debug::DebugViewPoint::Instance();

	InitialiseAssets();
}

TutorialGame::~TutorialGame() {
	//world->ClearAndErase();

	delete cubeMesh;
	delete sphereMesh;
	delete charMesh;
	delete enemyMesh;
	delete bonusMesh;

	delete basicTex;
	delete basicShader;

	delete physics;
	delete renderer;

	delete[] mazes;

	delete bulletPrefab;
}

void TutorialGame::InitWorld(InitMode mode) {
	score = 1000;

	delete[] mazes;
	mazes = nullptr;
	world->ClearAndErase();
	physics->Clear();

	player = AddPlayerToWorld(Vector3(0, 0, 0));
	switch (mode) {
		case InitMode::MAZE             : InitMazeWorld(20, 20, 20.0f)                            ; break;
		case InitMode::MIXED_GRID       : InitMixedGridWorld(15, 15, 3.5f, 3.5f)                  ; break;
		case InitMode::CUBE_GRID        : InitCubeGridWorld(15, 15, 3.5f, 3.5f, Vector3(1), true) ; break;
		case InitMode::OBB_GRID         : InitCubeGridWorld(15, 15, 3.5f, 3.5f, Vector3(1), false); break;
		case InitMode::SPHERE_GRID      : InitSphereGridWorld(15, 15, 3.5f, 3.5f, 1.0f)           ; break;
		case InitMode::BRIDGE_TEST      : InitBridgeConstraintTestWorld(10, 20, 30, false)        ; break;
		case InitMode::BRIDGE_TEST_ANG  : InitBridgeConstraintTestWorld(10, 20, 30, true)         ; break;
		case InitMode::PERFORMANCE_TEST : InitMixedGridWorld(30, 30, 10.0f, 10.0f)                ; break;
	}

	InitGameExamples();
	InitDefaultFloor();

	world->UpdateStaticTree();

	selectionObject = nullptr;

	InitCamera();
}

void TutorialGame::UpdateGame(float dt) {
	debugViewPoint->BeginFrame();
	debugViewPoint->MarkTime("Update");

	UpdateKeys();

	if (gameState == GameState::OnGoing) {
		Vector2 screenSize = Window::GetWindow()->GetScreenSize();

		Debug::Print(std::string("Score: ").append(std::to_string(score)), Vector2(5, 5), Vector4(1, 1, 0, 1));

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

		Debug::DrawLine(Vector3(), Vector3(0, 100, 0), Vector4(1, 0, 0, 1));

		if (player == nullptr) {
			SelectObject();
			MoveSelectedObject();
		}

		world->PreUpdateWorld();

		world->UpdateWorld(dt);
		renderer->Update(dt);
		physics->Update(dt);

		world->PostUpdateWorld();

		if (score < 0) {
			gameState = GameState::Lose;
		} else if (score > 5000) {
			gameState = GameState::Win;
		}
	}
	debugViewPoint->FinishTime("Update");
	debugViewPoint->MarkTime("Render");
	renderer->Render();
	
	Debug::UpdateRenderables(dt);
	debugViewPoint->FinishTime("Render");
}

void TutorialGame::InitialiseAssets() {
	cubeMesh    = renderer->LoadMesh("cube.msh");
	sphereMesh  = renderer->LoadMesh("sphere.msh");
	charMesh    = renderer->LoadMesh("goat.msh");
	enemyMesh   = renderer->LoadMesh("goose.msh");
	npcMesh     = renderer->LoadMesh("Keeper.msh");
	bonusMesh   = renderer->LoadMesh("Sphere.msh");
	capsuleMesh = renderer->LoadMesh("capsule.msh");
	AssetLibrary::AddMesh("cube", cubeMesh);
	AssetLibrary::AddMesh("sphere", sphereMesh);
	AssetLibrary::AddMesh("cube", charMesh);
	AssetLibrary::AddMesh("enemy", enemyMesh);
	AssetLibrary::AddMesh("bonus", bonusMesh);
	AssetLibrary::AddMesh("capsule", capsuleMesh);

	basicTex = renderer->LoadTexture("checkerboard.png");
	AssetLibrary::AddTexture("basic", basicTex);

	basicShader = renderer->LoadShader("scene.vert", "scene.frag");
	AssetLibrary::AddShader("basic", basicShader);

	InitialisePrefabs();
}

void TutorialGame::InitialisePrefabs() {
	float bulletRadius = 0.1f;

	bulletPrefab = new Bullet(*world);

	bulletPrefab->SetBoundingVolume((CollisionVolume*) new SphereVolume(bulletRadius, CollisionLayer::PlayerProj));
	bulletPrefab->GetTransform().SetScale(Vector3(bulletRadius));

	bulletPrefab->SetRenderObject(new RenderObject(&bulletPrefab->GetTransform(), sphereMesh, nullptr, basicShader));
	bulletPrefab->SetPhysicsObject(new PhysicsObject(&bulletPrefab->GetTransform(), bulletPrefab->GetBoundingVolume()));

	bulletPrefab->GetRenderObject()->SetColour(Vector4(1, 0.5f, 0.8f, 1.0f));

	bulletPrefab->GetPhysicsObject()->SetInverseMass(1.0f);
	bulletPrefab->GetPhysicsObject()->SetGravWeight(0.1f);
	bulletPrefab->GetPhysicsObject()->InitCapsuleInertia();

	AssetLibrary::AddPrefab("bullet", bulletPrefab);
}

void TutorialGame::InitCamera() {
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(1000.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
	lockedObject = nullptr;
}

void TutorialGame::UpdateKeys() {
	switch (gameState) {
	case GameState::OnGoing: default:
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)) {
			gameState = GameState::Paused;
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1)) {
			InitWorld(InitMode::MAZE);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F2)) {
			InitWorld(InitMode::MIXED_GRID);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F3)) {
			InitWorld(InitMode::CUBE_GRID);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F4)) {
			InitWorld(InitMode::OBB_GRID);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F5)) {
			InitWorld(InitMode::SPHERE_GRID);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F6)) {
			InitWorld(InitMode::BRIDGE_TEST);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F7)) {
			InitWorld(InitMode::BRIDGE_TEST_ANG);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F8)) {
			InitWorld(InitMode::PERFORMANCE_TEST);
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10)) {
			world->ShuffleConstraints(false);
		}

		if (lockedObject) {
			LockedObjectMovement();
		} else {
			DebugObjectMovement();
		}
		break;
		case GameState::Paused:
			Debug::Print("Press [Escape] to resume", Vector2(5, 80), Vector4(1, 1, 1, 1));
			Debug::Print("Press [q] to quit", Vector2(5, 90), Vector4(1, 1, 1, 1));

			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q)) {
				gameState = GameState::Quit;
			}
			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)) {
				gameState = GameState::OnGoing;
			}
			break;
		case GameState::Win:
			Debug::Print("You Win!", Vector2(5, 80), Vector4(0, 1, 0, 1));
			Debug::Print("Press [Space] to play again", Vector2(5, 90), Vector4(1, 1, 1, 1));

			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE)) {
				InitWorld(InitMode::MAZE);
				gameState = GameState::OnGoing;
			}
			break;
		case GameState::Lose:
			Debug::Print("You got Goosed!", Vector2(5, 80), Vector4(1, 0, 0, 1));
			Debug::Print("Press [Space] to play again", Vector2(5, 90), Vector4(1, 1, 1, 1));

			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE)) {
				InitWorld(InitMode::MAZE);
				gameState = GameState::OnGoing;
			}
			break;
	}
}

void TutorialGame::InitGameExamples() {
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
	GameObject* floor = new GameObject(*world, "Floor");

	Vector3 floorSize = Vector3(500, 2, 500);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetStatic();
	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(floor);

	return floor;
}

GameObject* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass) {
	static int id = 0;
	GameObject* sphere = new GameObject(*world, std::string("Sphere").append(std::to_string(id++)));

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* TutorialGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass, bool axisAligned) {
	static int id = 0;
	GameObject* cube = new GameObject(*world, std::string("Cube").append(std::to_string(id++)));

	cube->SetBoundingVolume(axisAligned ? ((CollisionVolume*)new AABBVolume(dimensions)) : ((CollisionVolume*)new OBBVolume(dimensions)));

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
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
	GameObject* capsule = new GameObject(*world, std::string("Capsule").append(std::to_string(id++)));

	Vector3 capsuleSize = Vector3(radius, halfHeight, radius);
	CapsuleVolume* volume = new CapsuleVolume(halfHeight, radius);
	capsule->SetBoundingVolume((CollisionVolume*)volume);

	capsule->GetTransform()
		.SetScale(capsuleSize)
		.SetPosition(position);

	capsule->SetRenderObject(new RenderObject(&capsule->GetTransform(), capsuleMesh, basicTex, basicShader));
	capsule->SetPhysicsObject(new PhysicsObject(&capsule->GetTransform(), capsule->GetBoundingVolume()));

	capsule->GetPhysicsObject()->SetInverseMass(inverseMass);
	capsule->GetPhysicsObject()->InitCapsuleInertia();

	world->AddGameObject(capsule);

	return capsule;
}

StateGameObject* TutorialGame::AddStateObjectToWorld(const Vector3& position) {
	static int id = 0;
	StateGameObject* sgo = new StateGameObject(*world, std::string("StateGameObject").append(std::to_string(id++)));
	SphereVolume* volume = new SphereVolume(1.0f);

	sgo->SetBoundingVolume((CollisionVolume*)volume);
	sgo->SetRenderObject(new RenderObject(&sgo->GetTransform(), sphereMesh, basicTex, basicShader));
	sgo->SetPhysicsObject(new PhysicsObject(&sgo->GetTransform(), sgo->GetBoundingVolume()));

	sgo->GetTransform().SetPosition(position);

	sgo->GetPhysicsObject()->SetInverseMass(5.0f);
	sgo->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(sgo);

	return sgo;
}

PlayerObject* TutorialGame::AddPlayerToWorld(const Vector3& position, bool cameraFollow) {
	static int id = 0;

	PlayerObject* character = new PlayerObject(*world, id++, score);
	SphereVolume* volume = new SphereVolume(1.0f, CollisionLayer::Player);

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(1, 1, 1))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), charMesh, nullptr, basicShader));
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
	EnemyObject* enemy = new EnemyObject(*world, *player, navMap);
	SphereVolume* volume = new SphereVolume(1.0f, CollisionLayer::Enemy);

	enemy->SetBoundingVolume((CollisionVolume*)volume);

	enemy->GetTransform()
		.SetScale(Vector3(1.0f))
		.SetPosition(position);

	enemy->SetRenderObject(new RenderObject(&enemy->GetTransform(), enemyMesh, nullptr, basicShader));
	enemy->SetPhysicsObject(new PhysicsObject(&enemy->GetTransform(), enemy->GetBoundingVolume()));

	enemy->GetRenderObject()->SetColour(Vector4(1, 0.9f, 0.8f, 1));

	enemy->GetPhysicsObject()->SetInverseMass(1);
	enemy->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(enemy);

	return enemy;
}

NPCObject* TutorialGame::AddNPCToWorld(const Vector3& position) {
	NPCObject* npc = new NPCObject(*world);
	CapsuleVolume* volume = new CapsuleVolume(1.5f, 1.0f);

	npc->SetBoundingVolume((CollisionVolume*)volume);

	npc->GetTransform()
		.SetScale(Vector3(2.0f))
		.SetPosition(position);

	npc->SetRenderObject(new RenderObject(&npc->GetTransform(), npcMesh, nullptr, basicShader));
	npc->SetPhysicsObject(new PhysicsObject(&npc->GetTransform(), npc->GetBoundingVolume()));

	npc->GetRenderObject()->SetColour(Vector4(1, 1, 0.8f, 1));

	npc->GetPhysicsObject()->SetInverseMass(1);
	npc->GetPhysicsObject()->InitCapsuleInertia();

	world->AddGameObject(npc);

	return npc;
}

GameObject* TutorialGame::AddBonusToWorld(const Vector3& position) {
	GameObject* bonus = new BonusObject(*world);
	SphereVolume* volume = new SphereVolume(1.5f);

	bonus->SetBoundingVolume((CollisionVolume*)volume);

	bonus->GetTransform()
		.SetScale(Vector3(1))
		.SetPosition(position);

	bonus->SetRenderObject(new RenderObject(&bonus->GetTransform(), bonusMesh, nullptr, basicShader));
	bonus->SetPhysicsObject(new PhysicsObject(&bonus->GetTransform(), bonus->GetBoundingVolume(), true));

	bonus->GetRenderObject()->SetColour(Vector4(1, 0.2f, 0.2f, 1));

	bonus->GetPhysicsObject()->SetInverseMass(0.0f);
	bonus->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(bonus);

	return bonus;
}

GameObject* TutorialGame::AddTriggerToWorld(const Vector3& position, float size) {
	GameObject* trigger = new GameObject(*world, "Trigger");

	SphereVolume* volume = new SphereVolume(size);
	trigger->SetBoundingVolume((CollisionVolume*)volume);
	trigger->GetTransform()
		.SetScale(Vector3(size))
		.SetPosition(position);

	trigger->SetRenderObject(new RenderObject(&trigger->GetTransform(), sphereMesh, nullptr, basicShader));
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
