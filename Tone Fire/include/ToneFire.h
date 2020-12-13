#pragma once
#include <string>
#include <vector>
#include "FMOD/core/fmod.hpp"
#include "FMOD/core/fmod_errors.h"
namespace ToneFire {
	class ToneFireFMOD;

	class Sound2D {
		friend ToneFireFMOD;
	public:
		Sound2D(std::string fileName,bool isStream = false, bool loop = false);
		Sound2D(std::string fileName, int fmodFlags);

		void Play();

		float GetVolume();
		void SetVolume(float v);

		bool IsMuted();
		void Mute();
		void UnMute();

		bool IsPlaying();


		std::string GetName() { return _name; }
	private:
		static ToneFireFMOD* _instance;

		FMOD::Sound* _sound;
		FMOD::Channel* _channel;
		float _volume = 0.0f;
		
		std::string _name;
	};

	class ToneFireFMOD {
		friend Sound2D;
	public:
		ToneFireFMOD(int maxChannels = 512,std::string defaultPath = "./Assets/Sounds/");
		~ToneFireFMOD();

		void Update();
		int GetChannelsPlaying();
	private:
		FMOD::Sound* _Create2DSound(std::string fileName,int flags = FMOD_2D);
		void _Play2DSound(Sound2D& sound);
		void _ErrorCheck(FMOD_RESULT result,std::string errorAt);
		bool _IsChannelPlaying(FMOD::Channel* c);
		FMOD::Channel* _GetFreeChannel();

		FMOD::System* _fmodSystem = nullptr;
		std::string _defaultPath;
		std::vector<FMOD::Channel*> _channels;
	};

	
}