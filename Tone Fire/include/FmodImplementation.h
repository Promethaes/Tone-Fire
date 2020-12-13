#pragma once
#include <string>
#include <vector>
#include "FMOD/core/fmod.hpp"
#include "FMOD/core/fmod_errors.h"
namespace ToneFire {
	class FMODImplementation;

	class Sound2D {
		friend FMODImplementation;
	public:
		Sound2D(std::string fileName);

		void Play();
		static FMODImplementation* instance;

	private:
		FMOD::Sound* _sound;
	};

	class FMODImplementation {
	public:
		FMODImplementation(int maxChannels = 512,std::string defaultPath = "./Assets/Sounds/");

		FMOD::Sound* Create2DSound(std::string fileName);
		void Play2DSound(Sound2D& sound);
		void Update();
	private:
		void _ErrorCheck(FMOD_RESULT result,std::string errorAt);

		FMOD::System* _fmodSystem = nullptr;
		std::string _defaultPath;
		std::vector<FMOD::Channel*> _channels;
	};

	
}