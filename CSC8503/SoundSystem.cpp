#include "SoundSystem.h"

SoundSystem* SoundSystem::instance = NULL;

SoundSystem::SoundSystem(unsigned int channels) {
	mListener = NULL;
	mMasterVolume = 1.0f;

	mSndDevice = SoundDevice::Get();

	for (unsigned int i = 0; i < channels; ++i) {
		ALuint source;

		alGenSources(1, &source);
		ALenum error = alGetError();

		if (error == AL_NO_ERROR) {
			mOALSources.emplace_back(new OALSource(source));
		}
		else {
			break;
		}
	}
	std::cout << "SoundSystem has " << mOALSources.size() << " channels available!" << std::endl;

}

SoundSystem::~SoundSystem() {
	
	delete mListener;
	for (vector < OALSource* >::iterator i = mOALSources.begin();
		i != mOALSources.end(); ++i) {
		alDeleteSources(1, &(*i)->source);
		delete (*i);
	}
	SoundDevice::Destroy();
}

