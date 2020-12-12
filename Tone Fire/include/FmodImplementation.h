#pragma once
#include <string>
#include "FMOD/core/fmod.hpp"
#include "FMOD/core/fmod_errors.h"
namespace ToneFire {
	class FMODImplementation {
	public:
		FMODImplementation(int maxChannels = 512);

	private:
		void _ErrorCheck(FMOD_RESULT result,std::string errorAt);

		FMOD::System* _fmodSystem = nullptr;
	};
}