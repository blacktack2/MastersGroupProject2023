#pragma once
#include<vector>
#include<map>
#include<algorithm>

#include"SoundDevice.h"
#include "Transform.h"
#include"GameObject.h"

class SoundSource;

struct OALSource {
	ALuint source;
	bool inUse;

	OALSource(ALuint src) {
		source = src;
		inUse = false;
	}
};

class SoundSystem
{
public:
	static SoundSystem* Initialize(unsigned int channels = 32) {
		if (!instance) {
			instance = new SoundSystem(channels);
		}
		return instance;
	}
	static void Destroy() { 
		delete instance;
		instance = nullptr;
	}

	inline static SoundSystem* GetSoundSystem() { return instance; }

	void AddSoundSource(SoundSource* s) { mSources.push_back(s); }
	//void RemoveSoundSource(SoundSource* s);

	void SetListener(NCL::CSC8503::GameObject* listener) { mListener = listener; }


	void Update(float mSec);

	void SetMasterVolume(float value);


	//void PlaySounds();
	OALSource* GetSource(); // to be protected

protected:
	SoundSystem(unsigned int channels=32);
	~SoundSystem();

	void UpdateListener();
	//void UpdateTemporaryEmitters(float msec);

	void DetachSources(std::vector<SoundSource *>::iterator from, std::vector<SoundSource *>::iterator to);
	void AttachSources(std::vector<SoundSource *>::iterator from, std::vector<SoundSource *>::iterator to);

	void CullNodes();

	

	

	SoundDevice* mSndDevice;
	std::vector<OALSource*> mOALSources;
	std::vector<SoundSource*> mSources;
	std::vector<SoundSource*> mFrameSources;
	std::map<std::string, ALuint> mSounds;

	float mMasterVolume;
	
	NCL::CSC8503::GameObject* mListener=nullptr;

	static SoundSystem* instance;
};

