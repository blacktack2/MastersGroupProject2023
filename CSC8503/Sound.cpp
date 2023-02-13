#include "Sound.h"
#include "SoundErrorReport.h"
#include "Assets.h"
//#include "OggSound.h"





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

