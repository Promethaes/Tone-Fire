#include "ToneFire.h"
#include <iostream>
using namespace ToneFire;


ToneFire::ToneFireFMOD::ToneFireFMOD(int maxChannels, std::string defaultPath) :_defaultPath(defaultPath)
{
	FMOD_RESULT result = FMOD::System_Create(&_fmodSystem);
	_ErrorCheck(result, "Creating FMOD system");
	result = _fmodSystem->init(maxChannels, FMOD_INIT_NORMAL, 0);
	_fmodSystem->setSoftwareFormat(48000, FMOD_SPEAKERMODE::FMOD_SPEAKERMODE_5POINT1, 0);
	_ErrorCheck(result, "Initializing FMOD system");

	Sound2D::_instance = this;

	for (int i = 0; i < maxChannels; i++)
		_channels.push_back(nullptr);
}

ToneFire::ToneFireFMOD::~ToneFireFMOD()
{
	_fmodSystem->release();
}

FMOD::Sound* ToneFire::ToneFireFMOD::_Create2DSound(std::string fileName, int flags)
{
	FMOD::Sound* temp = NULL;
	FMOD_RESULT result = _fmodSystem->createSound((_defaultPath + fileName).c_str(), flags, 0, &temp);
	_ErrorCheck(result, "Creating Sound: " + fileName);

	return temp;
}

void ToneFire::ToneFireFMOD::_Play2DSound(Sound2D& sound)
{

	//if the channel exists and is not being used, use the sound's channel
	if (sound._channel != nullptr && !_IsChannelPlaying(sound._channel)) {
		FMOD_RESULT result = _fmodSystem->playSound(sound._sound, nullptr, false, &sound._channel);
		_ErrorCheck(result, "Playing Sound: " + sound._name);
		return;
	}

	FMOD::Channel* c = _GetFreeChannel();
	FMOD_RESULT result = _fmodSystem->playSound(sound._sound, nullptr, false, &c);
	_ErrorCheck(result, "Playing Sound: " + sound._name);
	sound._channel = c;
	sound._channel->getVolume(&sound._volume);
}

void ToneFire::ToneFireFMOD::Update()
{
	FMOD_RESULT result = _fmodSystem->update();
	_ErrorCheck(result, "Update");
}

int ToneFire::ToneFireFMOD::GetChannelsPlaying()
{
	int temp = 0;
	_fmodSystem->getChannelsPlaying(&temp);
	return temp;
}

void ToneFire::ToneFireFMOD::_ErrorCheck(FMOD_RESULT result, std::string errorAt)
{
	if (!(result != FMOD_OK))
		return;
	printf("FMOD has encountered an error: (%d) %s at %s\n", result, FMOD_ErrorString(result), errorAt.c_str());
}

bool ToneFire::ToneFireFMOD::_IsChannelPlaying(FMOD::Channel* c)
{
	bool playing = false;

	FMOD_RESULT result = c->isPlaying(&playing);
	if (result == FMOD_ERR_INVALID_HANDLE)
		return false;
	_ErrorCheck(result, "Checking if channel playing");

	return playing;
}

FMOD::Channel* ToneFire::ToneFireFMOD::_GetFreeChannel()
{
	//find an unoccupied or uninitialized channel
	int index = -1;
	for (int i = 0; i < _channels.size(); i++) {
		if (_channels[i] == nullptr || !_IsChannelPlaying(_channels[i]))
			index = i;
	}
	return _channels[index];
}


ToneFireFMOD* Sound2D::_instance = nullptr;
ToneFire::Sound2D::Sound2D(std::string fileName, bool isStream,bool loop) :_name(fileName)
{
	int flags = FMOD_2D;
	if (isStream && loop)
		flags = FMOD_2D | FMOD_CREATESTREAM | FMOD_LOOP_NORMAL;
	else if(isStream)
		flags = FMOD_2D | FMOD_CREATESTREAM;
	else if (loop)
		flags = FMOD_2D | FMOD_LOOP_NORMAL;

	_sound = _instance->_Create2DSound(fileName, flags);
}

ToneFire::Sound2D::Sound2D(std::string fileName, int fmodFlags)
{
	_sound = _instance->_Create2DSound(fileName, fmodFlags);
}

void ToneFire::Sound2D::Play()
{
	_instance->_Play2DSound(*this);
}



float ToneFire::Sound2D::GetVolume()
{
	return _volume;
}

void ToneFire::Sound2D::SetVolume(float v)
{
	if (_channel == nullptr)
		return;
	_channel->setVolume(v);
	_volume = v;
}

bool ToneFire::Sound2D::IsMuted()
{
	bool temp = false;

	_channel->getMute(&temp);
	return temp;
}

void ToneFire::Sound2D::Mute()
{
	_channel->setMute(true);
}

void ToneFire::Sound2D::UnMute()
{
	_channel->setMute(false);
}

bool ToneFire::Sound2D::IsPlaying()
{
	if (_channel == nullptr) {
#ifdef _DEBUG
		std::cout << "Channel is nullptr\n";
#endif
		return false;
	}
	else return _instance->_IsChannelPlaying(_channel);
}
