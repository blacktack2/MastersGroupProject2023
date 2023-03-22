#include "TestAudio.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <string>
#include "Sound.h"
#include "SoundDevice.h"
#include "SoundSource.h"
#include "SoundSystem.h"
#include "Assets.h"

#include "AssetLibrary.h"
#include "AssetLoader.h"
#include "Bullet.h"
#include "Debug.h"
#include "GameWorld.h"
#include "OrientationConstraint.h"
#include "PhysicsObject.h"
#include "PlayerObject.h"
#include "PositionConstraint.h"
#include "RenderObject.h"
#include "StateGameObject.h"
#include "TutorialGame.h"


#define OpenAL_ErrorCheck(message)\
{\
	ALenum error = alGetError(); \
	if (error != AL_NO_ERROR)\
	{\
	std::cerr << "OpenAL Error:" << error << " with call for " << #message << std::endl;\
		}\
}

#define alec(FUNCTION_CALL)\
FUNCTION_CALL;\
OpenAL_ErrorCheck(FUNCTION_CALL)

//vector<string> TestAudio::audioFiles;

void TestAudio::TestAudio1()
{
	//Setup Device and context

	//SoundDevice* sd = SoundDevice::get();
	SoundSystem* sndSystem = SoundSystem::Initialize();

	//Setup Listener

	alec(alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f));
	alec(alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f));
	ALfloat forwardAndUpVectors[] = {
		/*forward = */ 1.0f,0.0f,0.0f,
		/*up = */ 0.0f,1.0f,0.0f
	};

	alec(alListenerfv(AL_ORIENTATION, forwardAndUpVectors));


	//Load files
	std::string name = /*NCL::Assets::SOUNDSDIR+*/"coin21.wav";

	ALuint buffer = Sound::AddSound(name);
	


	std::cout << "Hello World!\n";

	//Setup Source

	SoundSource* sn = new SoundSource();
	OALSource* source = sndSystem->GetSource();
	std::cout << "Using source:" << source->source<<std::endl;
	sn->SetPosition(NCL::Maths::Vector3(1.0f, 1.0f, 0.0f));
	sn->SetLooping(true);
	


	//Play source
	sn->SetSoundBuffer(buffer);
	sn->AttachSource(source);
	sn->Update(1.0f);
	//sn->Play(buffer);

	int i;
	std::cin >> i;
	std::cout << "Played!\n";
	//alec(alDeleteSources(1, &monoSource));
	delete sn;
	Sound::DeleteSounds();
	SoundSystem::Destroy();
}

void TestAudio::TestAudio2()
{
	
	//TutorialGame* game1 = new TutorialGame();
	//game1->InitWorld(NCL::CSC8503::TutorialGame::InitMode::AUDIO_TEST);
	//w->GetTimer()->GetTimeDeltaSeconds(); //Clear the timer so we don't get a larget first dt!
	//while (w->UpdateWindow() && !g->IsQuit()) {
	//	float dt = w->GetTimer()->GetTimeDeltaSeconds();
	//	if (dt > 0.1f) {
	//		std::cout << "Skipping large time delta" << std::endl;
	//		continue; //must have hit a breakpoint or something to have a 1 second frame time!
	//	}
	//	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::PRIOR)) {
	//		w->ShowConsole(true);
	//	}
	//	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NEXT)) {
	//		w->ShowConsole(false);
	//	}

	//	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::T)) {
	//		w->SetWindowPosition(0, 0);
	//	}

	//	w->SetTitle("Gametech frame time:" + std::to_string(1000.0f * dt));

	//	g->UpdateGame(dt);
	//	//DisplayPathfinding();
	//}
	/*TestAudio::audioFiles.emplace_back("coin2.wav");
	TestAudio::audioFiles.emplace_back("random1.wav");
	TestAudio::audioFiles.emplace_back("random2.wav");*/

	


	

}
