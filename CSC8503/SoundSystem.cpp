#include "SoundSystem.h"
#include "SoundSource.h"
#include"SoundManager.h"
SoundSystem* SoundSystem::instance = NULL;

void SoundSystem::Update(float mSec)
{
	UpdateListener();

	for (std::vector<SoundSource*>::iterator i = mSources.begin(); i != mSources.end(); ++i) {
		mFrameSources.emplace_back((*i));
		(*i)->Update(mSec);
	}

	CullNodes();

	if (mFrameSources.size() > mOALSources.size()) {
		std::sort(mFrameSources.begin(), mFrameSources.end(), SoundSource::CompareNodesByPriority);

		DetachSources(mFrameSources.begin() + (mOALSources.size() + 1), mFrameSources.end());
		AttachSources(mFrameSources.begin(), mFrameSources.begin() + mOALSources.size());
	}
	else {
		AttachSources(mFrameSources.begin(), mFrameSources.end());
	}
	mFrameSources.clear();
	
}

void SoundSystem::SetMasterVolume(float value)
{
	value = std::max(0.0f, value);
	value = std::min(1.0f, value);
	mMasterVolume = value;
	alListenerf(AL_GAIN, mMasterVolume);
}

SoundSystem::SoundSystem(unsigned int channels) {
	mListener = NULL;
	mMasterVolume = 1.0f;

	mSndDevice = SoundDevice::Get();

	for (unsigned int i = 0; i < channels; ++i) {
		ALuint source;

		alGenSources(1, &source);
		ALenum error = alGetError();

		if (error == AL_NO_ERROR) {
			mOALSources.emplace_back(new OALSource(source));
		}
		else {
			break;
		}
	}
	std::cout << "SoundSystem has " << mOALSources.size() << " channels available!" << std::endl;

}

SoundSystem::~SoundSystem() {
	
	delete mListener;
	//check if it should be added elsewhere
	for (vector < SoundSource* >::iterator i = mSources.begin();
		i != mSources.end(); ++i) {
		delete (*i);
	}
	for (vector < OALSource* >::iterator i = mOALSources.begin();
		i != mOALSources.end(); ++i) {
		alDeleteSources(1, &(*i)->source);
		delete (*i);
	}
	SoundManager::DeleteSounds();
	
	SoundDevice::Destroy();
}

void SoundSystem::UpdateListener()
{
	if(mListener){
		NCL::Maths::Vector3 worldPos = mListener->GetTransform().GetGlobalMatrix().GetPositionVector();

		NCL::Maths::Vector3 dirUp[2];
		//forward vector
		dirUp[0] = mListener->GetTransform().GetGlobalOrientation()*NCL::Maths::Vector3(0,0,-1);
		//up vector
		dirUp[1] = mListener->GetTransform().GetGlobalOrientation()*NCL::Maths::Vector3(0,1,0);

		alListenerfv(AL_POSITION, (float*)&worldPos);
		alListenerfv(AL_ORIENTATION, (float*)&dirUp);
	}
}

void SoundSystem::DetachSources(std::vector<SoundSource*>::iterator from, std::vector<SoundSource*>::iterator to)
{
	for (std::vector<SoundSource*>::iterator i = from; i != to; ++i) {
		(*i)->DetachSource();
	}
}

void SoundSystem::AttachSources(std::vector<SoundSource*>::iterator from, std::vector<SoundSource*>::iterator to)
{
	for (std::vector<SoundSource*>::iterator i = from; i != to; ++i) {
		if(!(*i)->GetSource()) 
			(*i)->AttachSource(GetSource());
	}
}

void SoundSystem::CullNodes()
{
	for (std::vector<SoundSource*>::iterator i = mFrameSources.begin(); i != mFrameSources.end();) {
		SoundSource* s = (*i);

		float length;
		if (s->GetTarget()) {
			length = (mListener->GetTransform().GetGlobalPosition() - s->GetTarget()->GetTransform().GetGlobalPosition()).Length();
		}
		else {
			length = (mListener->GetTransform().GetGlobalPosition() - s->GetPosition()).Length();
		}

		if (length > s->GetMaxDistance() || !s->GetSoundBuffer() /*|| !s->isPlaying()*/) {
			s->DetachSource();
			i = mFrameSources.erase(i);
		}
		else {
			++i;
		}
	}
}

OALSource* SoundSystem::GetSource()
{
	for (vector<OALSource*>::iterator i = mOALSources.begin(); i != mOALSources.end(); i++) {
		OALSource* s = *i;
		if (!s->inUse) {
			return s;
		}
	}
	return NULL;
}

