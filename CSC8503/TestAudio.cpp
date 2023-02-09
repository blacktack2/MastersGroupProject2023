#include "TestAudio.h"
#include<AL/al.h>
#include<AL/alc.h>
#include <string>
#include "Sound.h"
#include "SoundDevice.h"
#include "SoundSource.h"
#include "SoundSystem.h"
#include "Assets.h"


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
	std::string name = NCL::Assets::SOUNDSDIR+"coin21.wav";

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
	sn->Play(buffer);

	int i;
	std::cin >> i;
	std::cout << "Played!\n";
	//alec(alDeleteSources(1, &monoSource));
	delete sn;
	Sound::DeleteSounds();
	SoundSystem::Destroy();
}
