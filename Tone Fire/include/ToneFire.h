#pragma once

#ifdef TONEFIRE_EXPORT
#define DLL_PART __declspec(dllexport)
#else
#define DLL_PART __declspec(dllimport)
#endif

#include <string>
#include <vector>
#include "FMOD/core/fmod.hpp"
#include "FMOD/core/fmod_errors.h"
namespace ToneFire {
	class DLL_PART ToneFireFMOD;

	class DLL_PART Listener {
		friend DLL_PART ToneFireFMOD;
	public:
		Listener(
			FMOD_VECTOR pos = { 0.0f,0.0f,0.0f },
			FMOD_VECTOR up = { 0.0f,1.0f,0.0f },
			FMOD_VECTOR forward = { 0.0f,0.0f,1.0f }
		);

		FMOD_VECTOR GetPosition();
		void SetPosition(FMOD_VECTOR pos);

	private:
		FMOD_VECTOR _position;
		FMOD_VECTOR _velocity;//Not yet implemented
		FMOD_VECTOR _up;
		FMOD_VECTOR _forward;
	};

	class DLL_PART Sound {
		friend DLL_PART ToneFireFMOD;
	public:
		
		Sound(std::string fileName, bool isStream = false, bool loop = false, bool is3D = false);
		Sound(std::string fileName, int fmodFlags);

		void Play();

		//Be sure to call Play before you try to get or set the volume
		float GetVolume();
		//Be sure to call Play before you try to get or set the volume
		void SetVolume(float v);

		bool IsMuted();
		void Mute();
		void UnMute();

		bool IsPlaying();

		void SetSoundPosition(const FMOD_VECTOR& pos);



		std::string GetName() { return _name; }
	private:
		bool _ChannelIsNullptr();

		static ToneFireFMOD* _instance;

		FMOD::Sound* _sound;
		FMOD::Channel* _channel = nullptr;
		float _volume = 0.0f;
		bool _is3D = false;

		std::string _name;
	};

	class DLL_PART ToneFireFMOD {
		friend DLL_PART Sound;
	public:
		ToneFireFMOD(int maxChannels = 512, 
			std::string defaultPath = "./Assets/Sounds/",
			Listener listener = Listener());
		~ToneFireFMOD();

		void Update();
		int GetChannelsPlaying();

	private:
		FMOD::Sound* _CreateSound(std::string fileName, int flags = FMOD_2D);
		
		void _PlaySound(Sound& sound);
		
		Listener _listener;

		bool _IsChannelPlaying(FMOD::Channel* c);
		FMOD::Channel* _GetFreeChannel();
		
		void _ErrorCheck(FMOD_RESULT result, std::string errorAt);

		FMOD::System* _fmodSystem = nullptr;
		std::string _defaultPath;
		std::vector<FMOD::Channel*> _channels;
	};


}