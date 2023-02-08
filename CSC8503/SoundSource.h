#pragma once
#include<AL/al.h>
#include"SoundSystem.h"
#include"GameObject.h"

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
	inline float GetPitch()  { return mPitch; }
	
	void SetGain(const float& gain) { mGain= gain; }
	inline float GetGain()  { return mGain; }
	
	void SetMaxDistance(const float& distance) { if(distance>=0.0f) mMaxDistance= distance; }
	inline float GetMaxDistance()  { return mMaxDistance; }

	void SetPosition(const NCL::Maths::Vector3& position) { mPosition = position; }
	inline NCL::Maths::Vector3 GetPosition() { return mPosition; }

	void SetVelocity(const NCL::Maths::Vector3& velocity) { mVelocity = velocity; }
	inline NCL::Maths::Vector3 GetVelocity() { return mVelocity; }

	void SetDirection(const NCL::Maths::Vector3& direction) { mDirection= direction; }
	inline NCL::Maths::Vector3 GetDirection() { return mDirection; }

	void SetTimeLeft(const float& timeLeft) { mTimeLeft = timeLeft; }
	inline float  GetTimeLeft()const { return mTimeLeft; }


	void SetLooping(const bool& looping) { mIsLooping = looping; }
	inline bool IsLooping() { return mIsLooping; }

	void SetSoundBuffer(const ALuint& soundBuffer);
	inline ALuint GetSoundBuffer() { return mSoundBuffer; }

	inline OALSource* GetSource()const { return mSource; }
	void SetSource(OALSource* source) { mSource =source; }

	void SetPriority(const SoundPriority& priority) { mPriority = (SoundPriority)priority; }
	inline SoundPriority GetPriority() { return mPriority; }

	void SetTarget(NCL::CSC8503::GameObject* target) { mTarget = target; }

	void UpdateSoundSourceProperties();
	void UpdateSoundSourcePhysics();

	bool isPlaying();

	void AttachSource(OALSource* s);
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

