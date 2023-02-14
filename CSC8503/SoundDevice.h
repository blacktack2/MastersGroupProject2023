#pragma once
#include<AL/al.h>
#include<AL/alc.h>
#include <iostream>



class SoundDevice
{
public:
	static SoundDevice* Get();
	static void Destroy() { if(instance) delete instance; }
private:
	SoundDevice();
	~SoundDevice();

	static SoundDevice * instance;
	ALCdevice* mALCDevice;
	ALCcontext* mALCContext;
};

