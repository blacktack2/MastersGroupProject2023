#pragma once

#pragma once

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <AL/al.h>
#include <vector>
#include <dr_lib/dr_wav.h>

struct ReadWavData {
	unsigned int channels = 0;
	unsigned int sampleRate = 0;
	drwav_uint64 totalPCMFrameCount = 0;
	std::vector<uint16_t> pcmData;

	drwav_uint64 getTotalSamples() { return totalPCMFrameCount * channels; }

};


class Sound {
	friend class SoundManager;
public:
	
	bool			IsStreaming() { return mStreaming; }		//Part 2
	virtual double	StreamData(ALuint	buffer, double timeLeft) { return 0.0f; }							//Part 2

	ReadWavData getWavData()const { return mWavData; }

	static ALuint	AddSound(std::string n);
	static Sound* GetSound(std::string name);

	static void		DeleteSounds();

	void			LoadFromWAV(std::string filename);


protected:
	Sound();
	virtual ~Sound(void);

	//void			LoadFromWAV(string filename);
	//void			LoadWAVChunkInfo(ifstream& file, string& name, unsigned int& size);

	ALuint			mBuffer;

	ReadWavData mWavData;

	bool  mStreaming;			//Part 2
	float length;

	static std::map<std::string, Sound*> sSounds;
};
