#include "AssetLibrary.h"
#include "Bullet.h"
#include "Bonus.h"
#include "Debug.h"
#include "GameWorld.h"
#include "GameGridManager.h"
#include "Maze.h"
#include "OrientationConstraint.h"
#include "PhysicsObject.h"
#include "PlayerObject.h"
#include "PositionConstraint.h"
#include "RenderObject.h"
#include "StateGameObject.h"
#include "TextureLoader.h"
#include "PaintRenderObject.h"
#include "TutorialGame.h"
#include "InkEffectManager.h"

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
	world = &GameWorld::instance();
	sunLight = world->AddLight(new Light({ 0, 0, 0, 0 }, { 1, 1, 1, 1 }, 0, { 0.9f, 0.4f, 0.1f }));
	world->AddLight(new Light({ 0.0f, 5.0f, -10.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }, 10.0f));
#ifdef USEVULKAN
	renderer = new GameTechVulkanRenderer(*world);
#else 
	renderer = new GameTechRenderer(*world);
#endif

	physics		= new PhysicsSystem(*world);

	forceMagnitude	= 10.0f;
	inSelectionMode = false;
	debugViewPoint = &paintHell::debug::DebugViewPoint::Instance();
	
	SoundSystem::Initialize();
	gridManager = &GameGridManager::instance();
	InitialiseAssets();
	InitWorld();
}

TutorialGame::~TutorialGame() {
	//world->ClearAndErase();

	delete cubeMesh;
	delete sphereMesh;
	delete charMesh;
	delete enemyMesh;
	delete bonusMesh;

	delete basicTex;

	delete physics;
	delete renderer;

	delete[] mazes;

	delete bulletPrefab;

	SoundSystem::Destroy();
}

void TutorialGame::InitWorld(InitMode mode) {
	score = 1000;

	delete[] mazes;
	mazes = nullptr;
	world->ClearAndErase();
	physics->Clear();
	gridManager->Clear();

	gridManager->AddGameGrid( new GameGrid( { 0,0,0 }, 300, 300, 2 ) );
	BuildLevel();
	player = AddPlayerToWorld(Vector3(0, 0, 0));
	testingBoss = AddBossToWorld({ 0, 5, -20 }, { 2,2,2 }, 1);
	testingBossBehaviorTree = new BossBehaviorTree(testingBoss, player);

	switch (mode) {
		default: InitGameExamples(); break;
		case InitMode::MAZE             : InitMazeWorld(20, 20, 20.0f)                            ; break;
		case InitMode::MIXED_GRID       : InitMixedGridWorld(1, 1, 3.5f, 3.5f)                  ; break;
		case InitMode::CUBE_GRID        : InitCubeGridWorld(15, 15, 3.5f, 3.5f, Vector3(1), true) ; break;
		case InitMode::OBB_GRID         : InitCubeGridWorld(15, 15, 3.5f, 3.5f, Vector3(1), false); break;
		case InitMode::SPHERE_GRID      : InitSphereGridWorld(15, 15, 3.5f, 3.5f, 1.0f)           ; break;
		case InitMode::BRIDGE_TEST      : InitBridgeConstraintTestWorld(10, 20, 30, false)        ; break;
		case InitMode::BRIDGE_TEST_ANG  : InitBridgeConstraintTestWorld(10, 20, 30, true)         ; break;
		case InitMode::PERFORMANCE_TEST : InitMixedGridWorld(30, 30, 10.0f, 10.0f)                ; break;
		case InitMode::AUDIO_TEST : InitGameExamples()                ; break;
	}

	//InitGameExamples();
	InitDefaultFloor();

	world->UpdateStaticTree();

	selectionObject = nullptr;

	InitCamera();
}



void TutorialGame::UpdateGame(float dt) {
	GameState gameState = gameStateManager->GetGameState();

	debugViewPoint->BeginFrame();
	debugViewPoint->MarkTime("Update");

	UpdateKeys();
	static bool moveSun = false;
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM0)) {
		moveSun = !moveSun;
	}
	if (moveSun) {
		float runtime = world->GetRunTime();
		sunLight->direction = Vector3(std::sin(runtime), std::cos(runtime), 0.0f);
		renderer->GetSkyboxPass().SetSunDir(sunLight->direction);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM1)) {
		renderer->GetCombinePass().SetRenderMode(RenderMode::Default);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM2)) {
		renderer->GetCombinePass().SetRenderMode(RenderMode::Normals);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM3)) {
		renderer->GetCombinePass().SetRenderMode(RenderMode::Depth);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM4)) {
		renderer->GetCombinePass().SetRenderMode(RenderMode::Diffuse);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM5)) {
		renderer->GetCombinePass().SetRenderMode(RenderMode::DiffuseLight);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM6)) {
		renderer->GetCombinePass().SetRenderMode(RenderMode::SpecularLight);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::G)) {
			renderer->SetGamma(renderer->GetGamma() - 0.1f);
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::H)) {
			renderer->SetHDRExposure(renderer->GetHDRExposure() - 0.02f);
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::C)) {
			renderer->SetBloomAmount(renderer->GetBloomAmount() - 1);
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::B)) {
			renderer->SetBloomBias(renderer->GetBloomBias() - 0.02f);
		}
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::G)) {
			renderer->SetGamma(renderer->GetGamma() + 0.1f);
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::H)) {
			renderer->SetHDRExposure(renderer->GetHDRExposure() + 0.02f);
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::C)) {
			renderer->SetBloomAmount(renderer->GetBloomAmount() + 1);
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::B)) {
			renderer->SetBloomBias(renderer->GetBloomBias() + 0.02f);
		}
	}
	static bool doMain = true;
	static bool doPost = true;
	static bool doOver = true;
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::J)) {
		renderer->EnableRenderScene(doMain = !doMain);
		renderer->UpdatePipeline();
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::K)) {
		renderer->EnablePostProcessing(doPost = !doPost);
		renderer->UpdatePipeline();
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::L)) {
		renderer->EnableRenderOverlay(doOver = !doOver);
		renderer->UpdatePipeline();
	}

	static bool doBloom = true;
	static bool doHDR = true;
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::B) && Window::GetKeyboard()->KeyPressed(KeyboardKeys::BACK)) {
		renderer->EnablePostPass("Bloom", doBloom = !doBloom);
		renderer->UpdatePipeline();
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::H) && Window::GetKeyboard()->KeyPressed(KeyboardKeys::BACK)) {
		renderer->EnablePostPass("HDR", doHDR = !doHDR);
		renderer->UpdatePipeline();
	}
	Debug::Print(std::string("Gamma: ").append(std::to_string(renderer->GetGamma())), Vector2(0, 30.0f));
	if (doHDR) {
		Debug::Print(std::string("HDR:   ").append(std::to_string(renderer->GetHDRExposure())), Vector2(0, 35.0f));
	} else {
		Debug::Print(std::string("HDR:   Disabled"), Vector2(0, 35.0f));
	}
	if (doBloom) {
		Debug::Print(std::string("Bloom -> Amount: ").append(std::to_string(renderer->GetBloomAmount())), Vector2(0, 40.0f));
		Debug::Print(std::string("      -> Bias:   ").append(std::to_string(renderer->GetBloomBias())), Vector2(0, 45.0f));
	} else {
		Debug::Print(std::string("Bloom -> Disabled"), Vector2(0, 40.0f));
	}
	Debug::Print(std::string("Render -> Scene:   ").append(doMain ? "Enabled" : "Disabled"), Vector2(0, 50.0f));
	Debug::Print(std::string("       -> Post:    ").append(doPost ? "Enabled" : "Disabled"), Vector2(0, 55.0f));
	Debug::Print(std::string("       -> Overlay: ").append(doOver ? "Enabled" : "Disabled"), Vector2(0, 60.0f));
	SoundSystem::GetSoundSystem()->Update(dt);

	if (gameState == GameState::OnGoing) {
		UpdateStateOngoing(dt);
	}
	debugViewPoint->FinishTime("Update");
	debugViewPoint->MarkTime("Render");
	renderer->Render();
	
	Debug::UpdateRenderables(dt);
	debugViewPoint->FinishTime("Render");

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::PLUS))
	{
		testingBoss->SetHealth(100);
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::MINUS))
	{
		testingBoss->SetHealth(10);
	}
}

void TutorialGame::UpdateStateOngoing(float dt) {
	Vector2 screenSize = Window::GetWindow()->GetScreenSize();

	Debug::Print(std::string("health: ").append(std::to_string((int)player->GetHealth()->GetHealth())), Vector2(5, 5), Vector4(1, 1, 0, 1));

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

	//Debug::DrawLine(Vector3(), Vector3(0, 100, 0), Vector4(1, 0, 0, 1));

	if (player == nullptr) {
		SelectObject();
		MoveSelectedObject();
	}

	world->PreUpdateWorld();

	world->UpdateWorld(dt);
	renderer->Update(dt);
	physics->Update(dt);

	world->PostUpdateWorld();

	gridManager->Update(dt);
	//UpdateHealingKit();
	testingBossBehaviorTree->update();
	RenderBombsReleasedByBoss();
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::PLUS))
	{
		testingBoss->SetHealth(100);
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::MINUS))
	{
		testingBoss->SetHealth(10);
	}
	if (gameLevel->GetShelterTimer() > 20.0f)
	{
		gameLevel->SetShelterTimer(0.0f);
		BuildLevel();							// rebuild Shelters that has been destroyed
	}
}

void TutorialGame::InitialiseAssets() {
	cubeMesh    = renderer->LoadMesh("cube.msh");
	sphereMesh  = renderer->LoadMesh("sphere.msh");
	charMesh    = renderer->LoadMesh("goat.msh");
	enemyMesh   = renderer->LoadMesh("goose.msh");
	npcMesh     = renderer->LoadMesh("Keeper.msh");
	bonusMesh   = renderer->LoadMesh("Sphere.msh");
	capsuleMesh = renderer->LoadMesh("capsule.msh");

	// TODO
	pillarMesh = renderer->LoadMesh("cube.msh");
	fenceMesh = renderer->LoadMesh("cube.msh");
	shelterMesh = renderer->LoadMesh("cube.msh");

	AssetLibrary::AddMesh("cube", cubeMesh);
	AssetLibrary::AddMesh("sphere", sphereMesh);
	AssetLibrary::AddMesh("cube", charMesh);
	AssetLibrary::AddMesh("enemy", enemyMesh);
	AssetLibrary::AddMesh("bonus", bonusMesh);
	AssetLibrary::AddMesh("capsule", capsuleMesh);

	basicTex = renderer->LoadTexture("checkerboard.png");
	//inkableTex = renderer->LoadTexture("brick.tga");						/////////
	healingKitTex = renderer->LoadTexture("HealingKit.png");				/////////
	//noiseTex = renderer->LoadTexture("Perlin.png");							/////////
	//AssetLibrary::AddTexture("inkable", inkableTex);						/////////
	AssetLibrary::AddTexture("noise", noiseTex);							/////////
	AssetLibrary::AddTexture("basic", basicTex);
	OGLShader* shader = (OGLShader*)renderer->LoadShader("modelDefault.vert", "modelPaintTexture.frag");
	AssetLibrary::AddShader("paint", shader);
	renderer->GetModelPass().AddModelShader(shader);

	//basicShader = renderer->LoadShader("scene.vert", "scene.frag");			/////////
	//inkableShader = renderer->LoadShader("inkable.vert", "inkable.frag");	/////////
	//AssetLibrary::AddShader("inkable", inkableShader);						/////////
	//AssetLibrary::AddShader("basic", basicShader);							/////////

	InitialisePrefabs();
}

void TutorialGame::InitialisePrefabs() {
	float bulletRadius = 0.2f;

	bulletPrefab = new Bullet();

	bulletPrefab->SetBoundingVolume((CollisionVolume*) new SphereVolume(bulletRadius, CollisionLayer::PlayerProj));
	bulletPrefab->GetTransform().SetScale(Vector3(bulletRadius));

	bulletPrefab->SetRenderObject(new RenderObject(&bulletPrefab->GetTransform(), sphereMesh, nullptr, nullptr));
	bulletPrefab->SetPhysicsObject(new PhysicsObject(&bulletPrefab->GetTransform(), bulletPrefab->GetBoundingVolume()));

	bulletPrefab->GetRenderObject()->SetColour(Vector4(1, 0.5f, 0.8f, 1.0f));

	bulletPrefab->GetPhysicsObject()->SetInverseMass(1.0f);
	bulletPrefab->GetPhysicsObject()->SetGravWeight(1.0f);
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
	GameState gameState = gameStateManager->GetGameState();
	switch (gameState) {
		case GameState::OnGoing: default:
			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)) {
				gameStateManager->SetGameState(GameState::Paused);
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

			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F11)) {
				world->ShuffleConstraints(false);
			}

			if (lockedObject) {
				LockedObjectMovement();
			} else {
				DebugObjectMovement();
			}
			break;

		case GameState::Paused:
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);

			Debug::Print("Press [Escape] to resume", Vector2(5, 80), Vector4(1, 1, 1, 1));
			Debug::Print("Press [q] to quit", Vector2(5, 90), Vector4(1, 1, 1, 1));

			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q)) {
				gameStateManager->SetGameState(GameState::Quit);
			}
			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)) {
				gameStateManager->SetGameState(GameState::OnGoing);
				Window::GetWindow()->ShowOSPointer(false);
				Window::GetWindow()->LockMouseToWindow(true);
			}
			break;
		case GameState::Win:
			Debug::Print("You Win!", Vector2(5, 80), Vector4(0, 1, 0, 1));
			Debug::Print("Press [Space] to play again", Vector2(5, 90), Vector4(1, 1, 1, 1));

			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE)) {
				InitWorld();
				gameStateManager->SetGameState(GameState::OnGoing);
			}
			break;
		case GameState::Lose:
			Debug::Print("You got Goosed!", Vector2(5, 80), Vector4(1, 0, 0, 1));
			Debug::Print("Press [Space] to play again", Vector2(5, 90), Vector4(1, 1, 1, 1));

			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE)) {
				InitWorld();
				gameStateManager->SetGameState(GameState::OnGoing);
			}
			break;
	}
}

void TutorialGame::InitGameExamples()/*for audio testing*/ {
	
	InitDefaultFloor();
	//GameObject* obj1=AddCubeToWorld({ 55,0,0 }, { 2,2,2 });
	//obj1->GetRenderObject()->SetColour(Vector4(1.0f, 1.0f, 0.5f, 1.0f));

	//SoundSource* source1 = new SoundSource();
	//source1->SetLooping(true);
	//source1->SetTarget(obj1);
	//
	//ALuint snd1 = Sound::AddSound("coin2.wav");
	//ALuint snd2 = Sound::AddSound("random2.wav");
	//ALuint snd3 = Sound::AddSound("random1.wav");
	//source1->SetSoundBuffer(snd1);
	//if(SoundSystem::GetSoundSystem()->GetSource())
	//	source1->SetSource(SoundSystem::GetSoundSystem()->GetSource());

	//GameObject* obj2 = AddCubeToWorld({ 0,0,55 }, { 2,2,2 });
	//obj2->GetRenderObject()->SetColour(Vector4(1.0f, 1.0f, 0.5f, 1.0f));

	//SoundSource* source2 = new SoundSource();
	////source2->SetLooping(true);
	//source2->SetTarget(obj2);
	//source2->SetSoundBuffer(snd2);
	//if (SoundSystem::GetSoundSystem()->GetSource())
	//	source2->SetSource(SoundSystem::GetSoundSystem()->GetSource());

	//GameObject* obj3 = AddCubeToWorld({ 55,0,55 }, { 2,2,2 });
	//obj3->GetRenderObject()->SetColour(Vector4(1.0f, 1.0f, 0.5f, 1.0f));

	//SoundSource* source3 = new SoundSource();
	//source3->SetLooping(true);
	//source3->SetTarget(obj3);
	//source3->SetSoundBuffer(snd3);
	//if (SoundSystem::GetSoundSystem()->GetSource())
	//	source3->SetSource(SoundSystem::GetSoundSystem()->GetSource());

	//
	//SoundSystem::GetSoundSystem()->AddSoundSource(source1);
	//SoundSystem::GetSoundSystem()->AddSoundSource(source2);
	//SoundSystem::GetSoundSystem()->AddSoundSource(source3);
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
	floor = AddFloorToWorld(Vector3(0, -2, 0));
}

GameObject* TutorialGame::AddFloorToWorld(const Vector3& position) {
	GameObject* floor = new GameObject("Floor");

	Vector3 floorSize = Vector3(200, 2, 200);
	AABBVolume* volume = new AABBVolume(floorSize, CollisionLayer::PaintAble);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position);

	PaintRenderObject* render = new PaintRenderObject(&floor->GetTransform(), cubeMesh, basicTex);
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

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, nullptr));
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

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, nullptr));
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

	capsule->SetRenderObject(new RenderObject(&capsule->GetTransform(), capsuleMesh, basicTex, nullptr));
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
	sgo->SetRenderObject(new RenderObject(&sgo->GetTransform(), sphereMesh, basicTex, nullptr));
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

	character->SetRenderObject(new RenderObject(&character->GetTransform(), charMesh, nullptr, nullptr));
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

	enemy->SetRenderObject(new RenderObject(&enemy->GetTransform(), enemyMesh, nullptr, nullptr));
	enemy->SetPhysicsObject(new PhysicsObject(&enemy->GetTransform(), enemy->GetBoundingVolume()));

	enemy->GetRenderObject()->SetColour(Vector4(1, 0.9f, 0.8f, 1));

	enemy->GetPhysicsObject()->SetInverseMass(1);
	enemy->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(enemy);

	return enemy;
}

Boss* TutorialGame::AddBossToWorld(const Vector3& position, Vector3 dimensions, float inverseMass)
{
	Boss* boss = new Boss(*gameGrid);

	boss->SetBoundingVolume((CollisionVolume*)new AABBVolume(dimensions));

	boss->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	boss->SetRenderObject(new RenderObject(&boss->GetTransform(), cubeMesh, nullptr, nullptr));

	boss->GetRenderObject()->SetColour({ 1,1,0,1 });
	boss->SetPhysicsObject(new PhysicsObject(&boss->GetTransform(), boss->GetBoundingVolume()));

	boss->GetPhysicsObject()->SetInverseMass(inverseMass);
	boss->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(boss);

	return boss;
}

void TutorialGame::BuildLevel()
{
	float interval = 5.0f;
	Vector3 dimensions{ interval / 2.0f, 1.0f, interval / 2.0f };	// TODO: figure out the right scale
	gameLevel = new GameLevel{};
	gameLevel->AddRectanglarLevel("BasicLevel.txt", { -100,0,-70 }, interval);
	world->AddGameObject(gameLevel);

	for (auto& object : gameLevel->GetGameStuffs())
	{
		if (object.HasDestroyed())
		{
			object.Destroy(false);
			if (object.objectType == ObjectType::Pillar)
			{
				Obstacle* pillar = new Obstacle{ &object, true };
				dimensions.y = 5.0f;
				pillar->SetBoundingVolume((CollisionVolume*)new AABBVolume(dimensions));
				pillar->GetTransform()
					.SetPosition(object.worldPos + Vector3{ 0,5,0 })	// TODO: change height if necessary
					.SetScale(dimensions * 2);
				pillar->SetRenderObject(new RenderObject(&pillar->GetTransform(), pillarMesh, nullptr, nullptr));
				pillar->SetPhysicsObject(new PhysicsObject(&pillar->GetTransform(), pillar->GetBoundingVolume()));
				pillar->GetPhysicsObject()->SetInverseMass(0);
				pillar->GetPhysicsObject()->InitCubeInertia();
				world->AddGameObject(pillar);
			}
			if (object.objectType == ObjectType::Fence)
			{
				Obstacle* fence = new Obstacle{ &object, true };
				dimensions.y = 3.0f;
				fence->SetBoundingVolume((CollisionVolume*)new AABBVolume(dimensions));
				fence->GetTransform()
					.SetPosition(object.worldPos)	// TODO: change height if necessary
					.SetScale(dimensions * 2);
				fence->SetRenderObject(new RenderObject(&fence->GetTransform(), fenceMesh, nullptr, nullptr));		// TODO: change to the right Mesh
				fence->SetPhysicsObject(new PhysicsObject(&fence->GetTransform(), fence->GetBoundingVolume()));
				fence->GetPhysicsObject()->SetInverseMass(0);
				fence->GetPhysicsObject()->InitCubeInertia();
				world->AddGameObject(fence);
			}
			if (object.objectType == ObjectType::Shelter)
			{
				Obstacle* shelter = new Obstacle{ &object, false };
				dimensions.y = 3.0f;
				shelter->SetBoundingVolume((CollisionVolume*)new AABBVolume(dimensions));
				shelter->GetTransform()
					.SetPosition(object.worldPos)	// TODO: change height if necessary
					.SetScale(dimensions * 2);
				shelter->SetRenderObject(new RenderObject(&shelter->GetTransform(), shelterMesh, nullptr, nullptr));
				shelter->SetPhysicsObject(new PhysicsObject(&shelter->GetTransform(), shelter->GetBoundingVolume()));
				shelter->GetPhysicsObject()->SetInverseMass(0);
				shelter->GetPhysicsObject()->InitCubeInertia();
				world->AddGameObject(shelter);
			}
		}
	}
}

void TutorialGame::RenderBombsReleasedByBoss()
{
	std::vector<Bomb*> bossBombs = testingBoss->GetBombsReleasedByBoss();
	for (auto& bomb : bossBombs)
	{
		if (bomb->GetRenderObject() == nullptr)
		{
			bomb->SetRenderObject(new RenderObject(&bomb->GetTransform(), sphereMesh, nullptr, nullptr));
		}
		bomb->GetRenderObject()->SetColour({ 0,0,1,1 });

		world->AddGameObject(bomb);
	}
	testingBoss->clearBombList();
	if (testingBoss->isUsingInkSea())
	{
		//floor->GetRenderObject()->enableInkSea(testingBoss->GetTransform().GetGlobalPosition());
	}
	else
	{
		//floor->GetRenderObject()->disableInkSea();
	}
}

HealingKit* TutorialGame::UpdateHealingKit()
{
	Vector3 dimension{ 1,1,1 };

	gameGrid->UpdateHealingKits(player, dimension);
	gameGrid->UpdateHealingKits(testingBoss, dimension);

	float period = 1.0f;

	if (gameGrid->GetHealingKitTimer() > period)
	{
		HealingKit* healingKit = new HealingKit();
		Vector3 position = gameGrid->GetRandomLocationToAddHealingKit(healingKit);	// this function resets the timer, and add the new healingKit to the list used to store all healing kits in gameGrid
		healingKit->GetTransform()
			.SetPosition(position)
			.SetScale(dimension * 2);

		healingKit->SetRenderObject(new RenderObject(&healingKit->GetTransform(), cubeMesh, basicTex, nullptr));

		world->AddGameObject(healingKit);

		return healingKit;
	}
	return nullptr;
}

NPCObject* TutorialGame::AddNPCToWorld(const Vector3& position) {
	NPCObject* npc = new NPCObject();
	CapsuleVolume* volume = new CapsuleVolume(1.5f, 1.0f);

	npc->SetBoundingVolume((CollisionVolume*)volume);

	npc->GetTransform()
		.SetScale(Vector3(2.0f))
		.SetPosition(position);

	npc->SetRenderObject(new RenderObject(&npc->GetTransform(), npcMesh, nullptr, nullptr));
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

	bonus->SetRenderObject(new RenderObject(&bonus->GetTransform(), bonusMesh, nullptr, nullptr));
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

	trigger->SetRenderObject(new RenderObject(&trigger->GetTransform(), sphereMesh, nullptr, nullptr));
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
