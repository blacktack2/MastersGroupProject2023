#include "SoundDevice.h"
#include <AL/al.h>
#include <AL/alc.h>
#include "SoundErrorReport.h"

SoundDevice* SoundDevice::get() {
	static SoundDevice* s_snd_device = new SoundDevice();
	return s_snd_device;
}

SoundDevice::SoundDevice() {
	const ALCchar* defaultDeviceString = alcGetString(/*device*/ nullptr, ALC_DEFAULT_ALL_DEVICES_SPECIFIER);
	mALCDevice = alec(alcOpenDevice(defaultDeviceString));
	if (!mALCDevice) {
		std::cerr<<"Failed to get the Default device for OpenAL"<<std::endl;
	}
	mALCContext = alcCreateContext(mALCDevice, nullptr);
	if (!alcMakeContextCurrent(mALCContext)) {
		std::cerr << "Failed to make the OpenAL context the curent context" << std::endl;
	}

	const ALCchar* name = nullptr;
	alec(alcIsExtensionPresent(mALCDevice, "ALC_ENUMERATE_ALL_EXT"));
	name = alcGetString(mALCDevice, ALC_ALL_DEVICES_SPECIFIER);
	if (!name || alcGetError(mALCDevice) != AL_NO_ERROR)
		name = alcGetString(mALCDevice, ALC_DEVICE_SPECIFIER);
	std::cout<<"OpenAL opened device: "<<name<<std::endl;
	
	
}

SoundDevice::~SoundDevice() {
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(mALCContext);
	alcCloseDevice(mALCDevice);
}