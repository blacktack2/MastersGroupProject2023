#include "SoundManager.h"
#include "SoundErrorReport.h"
#include "Assets.h"
#include "Sound.h"
//#include "OggSound.h"




map<string, Sound*> SoundManager::sSounds;


ALuint SoundManager::AddSound(string name) {
	Sound* s = GetSound(name);

	if (!s) {
		string extension = name.substr(name.length() - 3, 3);

		if (extension == "wav") {
			s = new Sound();
			s->LoadFromWAV(NCL::Assets::SOUNDSDIR+name);
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

Sound* SoundManager::GetSound(string name) {
	map<string, Sound*>::iterator s = sSounds.find(name);
	return (s != sSounds.end() ? s->second : NULL);
}

void	SoundManager::DeleteSounds() {
	for (map<string, Sound*>::iterator i = sSounds.begin(); i != sSounds.end(); ++i) {
		delete i->second;
	}
}
