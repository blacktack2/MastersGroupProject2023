#pragma once
#include<AL/al.h>
#include"GameObject.h"


struct OALSource;

enum SoundPriority {
	SOUNDPRIORITY_LOW,
	SOUNDPRIORITY_MEDIUM,
	SOUNDPRIORITY_HIGH,
	SOUNDPRIORITY_ALWAYS
};

class SoundSource
{
public:
	SoundSource();
	SoundSource(ALuint sound);
	~SoundSource();
	
	void Play(ALuint bufferToPlay);
	void Pause();
	void Stop();

	void Reset();

	void SetPitch(const float& pitch) { if(pitch>=0.0f) mPitch = pitch; }
	float GetPitch()  { return mPitch; }
	
	void SetGain(const float& gain) { mGain= gain; }
	float GetGain()  { return mGain; }
	
	void SetMaxDistance(const float& distance) { if(distance>=0.0f) mMaxDistance= distance; }
	float GetMaxDistance()  { return mMaxDistance; }

	void SetPosition(const NCL::Maths::Vector3& position) { mPosition = position; }
	NCL::Maths::Vector3 GetPosition() { return mPosition; }

	void SetVelocity(const NCL::Maths::Vector3& velocity) { mVelocity = velocity; }
	NCL::Maths::Vector3 GetVelocity() { return mVelocity; }

	void SetDirection(const NCL::Maths::Vector3& direction) { mDirection= direction; }
	NCL::Maths::Vector3 GetDirection() { return mDirection; }

	void SetTimeLeft(const float& timeLeft) { mTimeLeft = timeLeft; }
	float  GetTimeLeft()const { return mTimeLeft; }

	NCL::CSC8503::GameObject* GetTarget()const { return mTarget; }

	void SetLooping(const bool& looping) { mIsLooping = looping; }
	bool IsLooping() { return mIsLooping; }

	void SetSoundBuffer(const ALuint& soundBuffer);
	ALuint GetSoundBuffer() { return mSoundBuffer; }

	inline OALSource* GetSource()const { return mSource; }
	void SetSource(OALSource* source) { mSource =source; }

	void SetPriority(const SoundPriority& priority) { mPriority = (SoundPriority)priority; }
	SoundPriority GetPriority() { return mPriority; }

	void SetTarget(NCL::CSC8503::GameObject* target) { mTarget = target; }

	void UpdateSoundSourceProperties();
	void UpdateSoundSourcePhysics();

	bool isPlaying();

	void AttachSource(OALSource *s);
	void DetachSource();

	static bool CompareNodesByPriority(SoundSource* a, SoundSource* b);
	void Update(float msec); 

protected:
	float mPitch;
	float mGain;
	float mMaxDistance;

	NCL::Maths::Vector3 mPosition;
	NCL::Maths::Vector3 mVelocity;
	NCL::Maths::Vector3 mDirection;

	bool mIsLooping;
	float mTimeLeft;

	NCL::CSC8503::GameObject* mTarget;

	OALSource* mSource;
	ALuint mSoundBuffer;
	SoundPriority mPriority;



};

