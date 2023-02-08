#include "SoundSource.h"
#include"SoundErrorReport.h"
#include<AL/alc.h>
SoundSource::SoundSource()
{
	Reset();
	
}

SoundSource::SoundSource(ALuint sound)
{
	Reset();
	mSoundBuffer = sound;
}


SoundSource::~SoundSource() {
	DetachSource();
}

void SoundSource::Reset() {
	mPitch = 1.0f;
	mGain = 1.0f;
	mMaxDistance = 50.0f;
	mIsLooping = false;
	mSoundBuffer = 0;
	mPriority = SoundPriority::SOUNDPRIORITY_LOW;
}

void SoundSource::SetSoundBuffer(const ALuint& soundBuffer)
{
	mSoundBuffer = soundBuffer;
	DetachSource();
	if (soundBuffer) {
		//mTimeLeft =
	}
}

void SoundSource::UpdateSoundSourceProperties()
{

	alSourcef(mSource->source, AL_PITCH, mPitch);
	alSourcef(mSource->source, AL_GAIN, mGain);
	alSourcei(mSource->source, AL_LOOPING, mIsLooping);
}

void SoundSource::UpdateSoundSourcePhysics()
{
	//alSourcefv(mSource.source, AL_POSITION, (float*)mPosition);
	//alSourcefv(mSource.source, AL_VELOCITY, (float*)mVelocity);
}

bool SoundSource::isPlaying()
{
	ALint sourceState;
	alGetSourcei(mSource->source, AL_SOURCE_STATE, &sourceState);
	return sourceState == AL_PLAYING;
}

void SoundSource::AttachSource(OALSource* s)
{
	mSource = s;
	if (!mSource) return;

	mSource->inUse = true;

	alSourcef(mSource->source, AL_MAX_DISTANCE, mMaxDistance);
	alSourcef(mSource->source, AL_REFERENCE_DISTANCE, mMaxDistance * 0.2f);
	Play(mSoundBuffer);
}

void SoundSource::DetachSource()
{
	if (!mSource)return;
	alSourcef(mSource->source, AL_GAIN, 0.0f);
	alSourceStop(mSource->source);
	alSourcei(mSource->source, AL_BUFFER, 0);

	mSource->inUse = false;
	mSource = NULL;
}

bool SoundSource::CompareNodesByPriority(SoundSource* a, SoundSource* b)
{
	return a->mPriority>b->mPriority;
}

void SoundSource::Update(float msec)
{
	if (mSource) {
		NCL::Maths::Vector3 pos;

		if (mTarget) {
			pos = mTarget->GetTransform().GetGlobalPosition();
		}
		else {
			pos = this->mPosition;
		}

		alSourcefv(mSource->source, AL_POSITION, (float*)&pos);
		alSourcef(mSource->source, AL_GAIN, mGain);
		alSourcei(mSource->source, AL_LOOPING, mIsLooping);
		alSourcef(mSource->source, AL_MAX_DISTANCE, mMaxDistance);
		alSourcef(mSource->source, AL_REFERENCE_DISTANCE, mMaxDistance*0.2f);

	}
}

void SoundSource::Play(ALuint bufferToPlay) {
	if (mSoundBuffer != bufferToPlay) {
		mSoundBuffer = bufferToPlay;
		alSourcei(mSource->source, AL_BUFFER, mSoundBuffer);
	}

	if (isPlaying()) return;
	alSourcePlay(mSource->source);
}

void SoundSource::Pause() {
	if (isPlaying()) alSourcePause(mSource->source);
}
void SoundSource::Stop() {
	if (isPlaying()) alSourceStop(mSource->source);
}