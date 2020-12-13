#include "FmodImplementation.h"
#include <iostream>
using namespace ToneFire;


ToneFire::FMODImplementation::FMODImplementation(int maxChannels, std::string defaultPath) :_defaultPath(defaultPath)
{
	FMOD_RESULT result = FMOD::System_Create(&_fmodSystem);
	_ErrorCheck(result, "Creating FMOD system");
	result = _fmodSystem->init(maxChannels, FMOD_INIT_NORMAL, 0);
	_ErrorCheck(result, "Initializing FMOD system");

	Sound2D::instance = this;

	for (int i = 0; i < maxChannels; i++)
		_channels.push_back(nullptr);
}

FMOD::Sound* ToneFire::FMODImplementation::Create2DSound(std::string fileName)
{
	FMOD::Sound* temp = NULL;
	FMOD_RESULT result = _fmodSystem->createSound((_defaultPath + fileName).c_str(), FMOD_2D, 0, & temp);
	_ErrorCheck(result, "Creating Sound: " + fileName);

	return temp;
}

void ToneFire::FMODImplementation::Play2DSound(Sound2D& sound)
{
	int index = -1;
	for (int i = 0; i < _channels.size(); i++) {
		if (_channels[i] == nullptr)
			index = i;
		else {
			bool playing = false;

			FMOD_RESULT result = _channels[i]->isPlaying(&playing);
			_ErrorCheck(result, "Checking if channel playing");

			if (!playing)
				index = i;
		}

	}

	_fmodSystem->playSound(sound._sound, nullptr, false, &_channels[index]);
}

void ToneFire::FMODImplementation::Update()
{
	FMOD_RESULT result = _fmodSystem->update();
	_ErrorCheck(result, "Update");
}

void ToneFire::FMODImplementation::_ErrorCheck(FMOD_RESULT result, std::string errorAt)
{
	if (!(result != FMOD_OK))
		return;
	printf("FMOD has encountered an error: (%d) %s %s", result, FMOD_ErrorString(result), errorAt.c_str());
}


FMODImplementation* Sound2D::instance = nullptr;
ToneFire::Sound2D::Sound2D(std::string fileName)
{
	_sound = instance->Create2DSound(fileName);
}

void ToneFire::Sound2D::Play()
{
	instance->Play2DSound(*this);
}
