#include "FmodImplementation.h"
#include <iostream>
using namespace ToneFire;
FMODImplementation::FMODImplementation(int maxChannels)
{
	FMOD_RESULT result = FMOD::System_Create(&_fmodSystem);
	_ErrorCheck(result, "Creating FMOD system");
	result = _fmodSystem->init(maxChannels, 0, 0);
	_ErrorCheck(result, "Initializing FMOD system");

}

void ToneFire::FMODImplementation::_ErrorCheck(FMOD_RESULT result, std::string errorAt)
{
	if (!(result != FMOD_OK))
		return;
	printf("FMOD has encountered an error: (%d) %s %s", result, FMOD_ErrorString(result), errorAt.c_str());
}
