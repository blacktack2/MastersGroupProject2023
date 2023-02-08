#pragma once
#include<AL/al.h>
#include<AL/alc.h>
#include <iostream>



class SoundDevice
{
public:
	static SoundDevice* get();
private:
	SoundDevice();
	~SoundDevice();


	ALCdevice* mALCDevice;
	ALCcontext* mALCContext;
};

