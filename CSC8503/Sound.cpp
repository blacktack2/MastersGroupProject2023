#include "Sound.h"
#include "SoundErrorReport.h"
//#include "OggSound.h"




map<string, Sound*> Sound::sSounds;


Sound::Sound() {
	mStreaming = false;		//Part 2
	mBuffer = 0;
}

Sound::~Sound(void) {
	alDeleteBuffers(1, &mBuffer);
}


void	Sound::LoadFromWAV(string filename) {

	drwav_int16* pSampleData = drwav_open_file_and_read_pcm_frames_s16(filename.c_str(), &mWavData.channels, &mWavData.sampleRate, &mWavData.totalPCMFrameCount, nullptr);
	if (pSampleData == NULL) {
		std::cerr << "failed to load audio file" << std::endl;
		drwav_free(pSampleData, nullptr); //todo use raii to clean this up
		return;
	}
	if (mWavData.getTotalSamples() > drwav_uint64(std::numeric_limits<size_t>::max()))
	{
		std::cerr << "too much data in file for 32bit addressed vector" << std::endl;
		drwav_free(pSampleData, nullptr);
		return;
	}
	mWavData.pcmData.resize(size_t(mWavData.getTotalSamples()));
	std::memcpy(mWavData.pcmData.data(), pSampleData, mWavData.pcmData.size() * /*twobytes_in_s16*/2);
	drwav_free(pSampleData, nullptr);

	length = (float)mWavData.pcmData.size() / (mWavData.channels* mWavData.totalPCMFrameCount * (mWavData.sampleRate / 8.0f)) * 1000.0f;

}

ALuint Sound::AddSound(string name) {
	Sound* s = GetSound(name);

	if (!s) {
		string extension = name.substr(name.length() - 3, 3);

		if (extension == "wav") {
			s = new Sound();
			s->LoadFromWAV(name);
			alec(alGenBuffers(1, &s->mBuffer));
			alec(alBufferData(s->mBuffer, s->mWavData.channels > 1 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, s->mWavData.pcmData.data(), s->mWavData.pcmData.size() * 2 /*two bytes per sample*/, s->mWavData.sampleRate));
		}
		/*else if (extension == "ogg") {
			OggSound* ogg = new OggSound();
			ogg->LoadFromOgg(name);

			s = ogg;
		}*/
		else {
			s = new Sound();
			cout << "Incompatible file extension '" << extension << "'!" << endl;
		}

		sSounds.insert(make_pair(name, s));
	}
	return s->mBuffer;
}

Sound* Sound::GetSound(string name) {
	map<string, Sound*>::iterator s = sSounds.find(name);
	return (s != sSounds.end() ? s->second : NULL);
}

void	Sound::DeleteSounds() {
	for (map<string, Sound*>::iterator i = sSounds.begin(); i != sSounds.end(); ++i) {
		delete i->second;
	}
}
