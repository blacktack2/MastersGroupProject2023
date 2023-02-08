#pragma once
#include<vector>
#include"SoundSource.h"
#include"SoundDevice.h"
#include<map>
#include<algorithm>

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
	static SoundSystem* get();
	static void DestroySoundSystem(SoundSystem* s);

	void Update(float mSec);
	void PlaySounds();


protected:
	SoundSystem(unsigned int channels=32);
	~SoundSystem();

	void UpdateListener();
	void UpdateTemporaryEmitters(float msec);

	void DetachSources(vector<SoundSource*>::iterator from, vector<SoundSource*>::iterator to);
	void AttachSources(vector<SoundSource*>::iterator from, vector<SoundSource*>::iterator to);

	void CullNodes();

	OALSource* GetSource();

	

	SoundDevice* mSndDevice;
	std::vector<SoundSource*> mSources;
	std::map<std::string, ALuint> mSounds;

	float mListenerGain;
	float mListenerPosition[3];
	float mListenerVelocity[3];


};

