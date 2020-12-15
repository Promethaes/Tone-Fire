#include "ToneFire.h"
#include <iostream>
using namespace ToneFire;


ToneFire::ToneFireFMOD::ToneFireFMOD(int maxChannels, std::string defaultPath, Listener listener)
	:_defaultPath(defaultPath), _listener(listener)
{
	FMOD_RESULT result = FMOD::System_Create(&_fmodSystem);
	_ErrorCheck(result, "Creating FMOD system");
	result = _fmodSystem->init(maxChannels, FMOD_INIT_NORMAL, 0);

	_fmodSystem->setSoftwareFormat(48000, FMOD_SPEAKERMODE::FMOD_SPEAKERMODE_STEREO, 0);
	_ErrorCheck(result, "Initializing FMOD system");

	Sound::_instance = this;

	for (int i = 0; i < maxChannels; i++)
		_channels.push_back(nullptr);


	//3D settings
	_fmodSystem->set3DSettings(1.0f, 1.0f, 1.0f);
	_fmodSystem->set3DListenerAttributes(0, &_listener.GetPosition(), 0, &_listener._forward, &_listener._up);

}

ToneFire::ToneFireFMOD::~ToneFireFMOD()
{
	_fmodSystem->release();
}

FMOD::Sound* ToneFire::ToneFireFMOD::_CreateSound(std::string fileName, int flags)
{
	FMOD::Sound* temp = NULL;
	
	std::string path = "http";
	std::size_t found = fileName.find(path);
	if (found != std::string::npos)
		path = fileName;
	else
		path = _defaultPath + fileName;

	FMOD_RESULT result = _fmodSystem->createSound((path).c_str(), flags, 0, &temp);
	_ErrorCheck(result, "Creating Sound: " + fileName);

	return temp;
}

void ToneFire::ToneFireFMOD::_PlaySound(Sound& sound)
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
	_fmodSystem->set3DListenerAttributes(0, &_listener._position, 0, &_listener._forward, &_listener._up);
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

	if (c == nullptr)
		return false;

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


ToneFireFMOD* Sound::_instance = nullptr;
ToneFire::Sound::Sound(std::string fileName, bool isStream, bool loop, bool is3D) :_name(fileName),_is3D(is3D)
{
	if (!is3D) {
		int flags = FMOD_2D;
		if (isStream && loop)
			flags = FMOD_2D | FMOD_CREATESTREAM | FMOD_LOOP_NORMAL;
		else if (isStream)
			flags = FMOD_2D | FMOD_CREATESTREAM;
		else if (loop)
			flags = FMOD_2D | FMOD_LOOP_NORMAL;

		_sound = _instance->_CreateSound(fileName, flags);
		return;
	}
	if (is3D) {
		int flags = FMOD_3D;
		if (isStream && loop)
			flags = FMOD_3D | FMOD_CREATESTREAM | FMOD_LOOP_NORMAL;
		else if (isStream)
			flags = FMOD_3D | FMOD_CREATESTREAM;
		else if (loop)
			flags = FMOD_3D | FMOD_LOOP_NORMAL;

		_sound = _instance->_CreateSound(fileName, flags);
	}
}

ToneFire::Sound::Sound(std::string fileName, int fmodFlags) : _name(fileName)
{
	if ((fmodFlags & FMOD_3D) == FMOD_3D)
		_is3D = true;
	_sound = _instance->_CreateSound(fileName, fmodFlags);
}

void ToneFire::Sound::Play()
{
	_instance->_PlaySound(*this);
}

float ToneFire::Sound::GetVolume()
{
	return _volume;
}

void ToneFire::Sound::SetVolume(float v)
{
	if (_ChannelIsNullptr())
		return;
	_channel->setVolume(v);
	_volume = v;
}

bool ToneFire::Sound::IsMuted()
{
	bool temp = false;
	if (_ChannelIsNullptr())
		return false;

	_channel->getMute(&temp);
	return temp;
}

void ToneFire::Sound::Mute()
{
	if (_ChannelIsNullptr())
		return;
	_channel->setMute(true);
}

void ToneFire::Sound::UnMute()
{
	if (_ChannelIsNullptr())
		return;
	_channel->setMute(false);
}

bool ToneFire::Sound::IsPlaying()
{
	if (_ChannelIsNullptr())
		return false;
	return _instance->_IsChannelPlaying(_channel);
}

void ToneFire::Sound::SetSoundPosition(const FMOD_VECTOR& pos)
{
	if (_ChannelIsNullptr())
		return;

	_channel->set3DAttributes(&pos, 0);
}

bool ToneFire::Sound::_ChannelIsNullptr()
{
	bool b = false;
	if (_channel == nullptr || _channel->isPlaying(&b) == FMOD_ERR_INVALID_HANDLE) {
#ifdef _DEBUG
		std::cout << _name + "'s Channel is nullptr or invalid. Be sure to call Play to initialize the channel before you attempt to modify it.\n";
#endif
		return true;
	}
	return false;
}

ToneFire::Listener::Listener(FMOD_VECTOR pos, FMOD_VECTOR up, FMOD_VECTOR forward)
	:_position(pos), _up(up), _forward(forward)
{
}

FMOD_VECTOR ToneFire::Listener::GetPosition()
{
	return _position;
}

void ToneFire::Listener::SetPosition(FMOD_VECTOR pos)
{
	_position = pos;
}
