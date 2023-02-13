#pragma once

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <AL/al.h>
#include <vector>
#include <dr_lib/dr_wav.h>

using namespace std;
class Sound;
class SoundManager {
public:


	static ALuint	AddSound(string n);
	static Sound* GetSound(string name);

	static void		DeleteSounds();



protected:
	SoundManager();
	~SoundManager(void);

	static map<string, Sound*> sSounds;


};
