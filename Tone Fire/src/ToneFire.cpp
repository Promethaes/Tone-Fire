#include "ToneFire.h"
#include <iostream>
using namespace ToneFire;


ToneFire::FMODCore::FMODCore(int maxChannels, const std::string& defaultPath, const Listener& listener)
	:_defaultPath(defaultPath), _listener(listener)
{
	//create and initialize the fmod system
	FMOD_RESULT result = FMOD::System_Create(&_fmodSystem);
	_ErrorCheck(result, "Creating FMOD system");

	_fmodSystem->setSoftwareFormat(48000, FMOD_SPEAKERMODE::FMOD_SPEAKERMODE_STEREO, 0);
	result = _fmodSystem->init(maxChannels, FMOD_INIT_NORMAL, 0);

	_ErrorCheck(result, "Initializing FMOD system");

	CoreSound::_instance = this;

	for (int i = 0; i < maxChannels; i++)
		_channels.push_back(nullptr);


	//3D settings
	_fmodSystem->set3DSettings(1.0f, 1.0f, 1.0f);
	_fmodSystem->set3DListenerAttributes(0, &_listener.GetPosition(), 0, &_listener._forward, &_listener._up);

}

ToneFire::FMODCore::~FMODCore()
{
	_fmodSystem->release();
}

FMOD::Sound* ToneFire::FMODCore::_CreateSound(std::string fileName, int flags)
{
	FMOD::Sound* temp = NULL;
	
	//determine if this is a local or remote asset, kinda broken tbh
	std::string path = "http";
	std::size_t found = fileName.find(path);
	if (found != std::string::npos)
		path = fileName;
	else
		path = _defaultPath + fileName;

	//create the sound
	FMOD_RESULT result = _fmodSystem->createSound((path).c_str(), flags, 0, &temp);
	_ErrorCheck(result, "Creating Sound: " + fileName);

	return temp;
}

void ToneFire::FMODCore::_PlaySound(CoreSound& sound)
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

void ToneFire::FMODCore::Update()
{
	_fmodSystem->set3DListenerAttributes(0, &_listener._position, 0, &_listener._forward, &_listener._up);
	FMOD_RESULT result = _fmodSystem->update();
	_ErrorCheck(result, "Update");
}

int ToneFire::FMODCore::GetChannelsPlaying()
{
	int temp = 0;
	_fmodSystem->getChannelsPlaying(&temp);
	return temp;
}

void ToneFire::FMODCore::_ErrorCheck(FMOD_RESULT result, std::string errorAt)
{
	if (!(result != FMOD_OK))
		return;
	printf("FMOD has encountered an error: (%d) %s at %s\n", result, FMOD_ErrorString(result), errorAt.c_str());
}

bool ToneFire::FMODCore::_IsChannelPlaying(FMOD::Channel* c)
{
	bool playing = false;

	if (c == nullptr)
		return false;

	FMOD_RESULT result = c->isPlaying(&playing);
	_ErrorCheck(result, "Checking if channel playing");
	if (result == FMOD_ERR_INVALID_HANDLE)
		return false;

	return playing;
}

FMOD::Channel* ToneFire::FMODCore::_GetFreeChannel()
{
	//find an unoccupied or uninitialized channel
	int index = -1;
	for (int i = 0; i < _channels.size(); i++) {
		if (_channels[i] == nullptr || !_IsChannelPlaying(_channels[i]))
			index = i;
	}
	return _channels[index];
}


FMODCore* CoreSound::_instance = nullptr;
ToneFire::CoreSound::CoreSound(const std::string& fileName, bool isStream, bool loop, bool is3D)
	:_name(fileName),_is3D(is3D)
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

ToneFire::CoreSound::CoreSound(const std::string& fileName, int fmodFlags) 
	: _name(fileName)
{
	if ((fmodFlags & FMOD_3D) == FMOD_3D)
		_is3D = true;
	_sound = _instance->_CreateSound(fileName, fmodFlags);
}

void ToneFire::CoreSound::Play()
{
	_instance->_PlaySound(*this);
}

float ToneFire::CoreSound::GetVolume()
{
	return _volume;
}

void ToneFire::CoreSound::SetVolume(float v)
{
	if (_ChannelIsNullptr())
		return;
	_channel->setVolume(v);
	_volume = v;
}

bool ToneFire::CoreSound::IsMuted()
{
	bool temp = false;
	if (_ChannelIsNullptr())
		return false;

	_channel->getMute(&temp);
	return temp;
}

void ToneFire::CoreSound::Mute()
{
	if (_ChannelIsNullptr())
		return;
	_channel->setMute(true);
}

void ToneFire::CoreSound::UnMute()
{
	if (_ChannelIsNullptr())
		return;
	_channel->setMute(false);
}

bool ToneFire::CoreSound::IsPlaying()
{
	if (_ChannelIsNullptr())
		return false;
	return _instance->_IsChannelPlaying(_channel);
}

void ToneFire::CoreSound::SetSoundPosition(const FMOD_VECTOR& pos)
{
	if (_ChannelIsNullptr())
		return;

	_channel->set3DAttributes(&pos, 0);
}

bool ToneFire::CoreSound::_ChannelIsNullptr()
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

FMOD_VECTOR& ToneFire::Listener::GetPosition()
{
	return _position;
}

void ToneFire::Listener::SetPosition(FMOD_VECTOR pos)
{
	_position = pos;
}

ToneFire::FMODStudio::FMODStudio(int maxChannels, const std::string& defaultPath, 
	const Listener& listener)
	:_defaultPath(defaultPath),_listener(listener)
{
	FMOD_RESULT result = FMOD::Studio::System::create(&_fmodSystem);
	_ErrorCheck(result, "Creating FMOD Studio System");
	FMOD::System* core = NULL;
	_fmodSystem->getCoreSystem(&core);
	core->setSoftwareFormat(48000, FMOD_SPEAKERMODE::FMOD_SPEAKERMODE_5POINT1, 0);

	result = _fmodSystem->initialize(maxChannels, FMOD_STUDIO_INIT_NORMAL, 0, 0);
	_ErrorCheck(result, "Initializing FMOD Studio System");

	StudioSound::_instance = this;

}

ToneFire::FMODStudio::~FMODStudio()
{
	_fmodSystem->release();
}

void ToneFire::FMODStudio::LoadBank(const std::string& bankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags)
{
	FMOD::Studio::Bank* bank = NULL;
	FMOD_RESULT result = _fmodSystem->loadBankFile((_defaultPath + bankName).c_str(), flags, &bank);
	_ErrorCheck(result, "Loading " + bankName);

}

FMOD::Studio::EventDescription* ToneFire::FMODStudio::_GetEventDescription(const std::string& eventName)
{
	FMOD::Studio::EventDescription* eventDesc = NULL;
	FMOD_RESULT result = _fmodSystem->getEvent(eventName.c_str(), &eventDesc);
	_ErrorCheck(result, "Loading " + eventName);

	return eventDesc;
}

void ToneFire::FMODStudio::Update()
{
	_atr.forward = _listener._forward;
	_atr.position = _listener._position;
	_atr.up = _listener._up;
	_atr.velocity = _listener._velocity;
	FMOD_RESULT result = _fmodSystem->setListenerAttributes(0, &_atr,0);
	_ErrorCheck(result, "set listener attributes");
	_fmodSystem->update();
}

void ToneFire::FMODStudio::_ErrorCheck(FMOD_RESULT result, std::string errorAt)
{
	if (!(result != FMOD_OK))
		return;
	printf("FMOD has encountered an error: (%d) %s at %s\n", result, FMOD_ErrorString(result), errorAt.c_str());
}



FMODStudio* StudioSound::_instance = nullptr;

ToneFire::StudioSound::StudioSound()
{
	_bankEventDescriptions.reserve(128);
	_bankEventInstances.reserve(128);
}

void ToneFire::StudioSound::LoadEvent(const std::string& eventName)
{
	_bankEventDescriptions[eventName] = _instance->_GetEventDescription(eventName);
	FMOD_RESULT result = _bankEventDescriptions[eventName]->createInstance(&_bankEventInstances[eventName]);
	_instance->_ErrorCheck(result, "Creating Event Instance: " + eventName);
}

void ToneFire::StudioSound::PlayEvent(const std::string& eventName)
{
	if (_bankEventDescriptions[eventName] == nullptr)
		LoadEvent(eventName);

	_bankEventInstances[eventName]->start();
}

void ToneFire::StudioSound::StopEvent(const std::string& eventName)
{
	if (_bankEventDescriptions[eventName] == nullptr)
		LoadEvent(eventName);
	_bankEventInstances[eventName]->stop(FMOD_STUDIO_STOP_IMMEDIATE);
}

void ToneFire::StudioSound::SetEventParameter(const std::string& eventName, const std::string& parameterName, float paramValue)
{
	if (_bankEventDescriptions[eventName] == nullptr)
		LoadEvent(eventName);
	FMOD_RESULT result = _bankEventInstances[eventName]->setParameterByName(parameterName.c_str(), paramValue);
	_instance->_ErrorCheck(result, "Setting " + parameterName + " of " + eventName);

}

void ToneFire::StudioSound::SetEventPosition(const std::string& eventName,const FMOD_VECTOR& pos)
{
	if (_bankEventDescriptions[eventName] == nullptr)
		LoadEvent(eventName);
	FMOD_3D_ATTRIBUTES atr;
	atr.forward =	forward;
	atr.position =	pos;
	atr.up =		up;
	atr.velocity =	_velocity;
	_bankEventInstances[eventName]->set3DAttributes(&atr);
}
