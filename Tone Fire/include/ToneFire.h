#pragma once

#ifdef TONEFIRE_EXPORT
#define DLL __declspec(dllexport)
#else
#define DLL __declspec(dllimport)
#endif

#include <string>
#include <vector>
#include <unordered_map>
#include "FMOD/fmod.hpp"
#include "FMOD/fmod_errors.h"
#include "FMOD/fmod_studio.hpp"
#include "FMOD/fmod_studio_common.h"
namespace ToneFire {
	class DLL FMODStudio;
	class DLL FMODCore;

	class DLL Listener {
		friend DLL FMODCore;
		friend DLL FMODStudio;
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

	class DLL CoreSound {
		friend DLL FMODCore;
	public:

		CoreSound(const std::string& fileName, bool isStream = false, bool loop = false, bool is3D = false);
		CoreSound(const std::string& fileName, int fmodFlags);

		void Play();

		float GetVolume();
		void SetVolume(float v);

		bool IsMuted();
		void Mute();
		void UnMute();

		bool IsPlaying();

		void SetSoundPosition(const FMOD_VECTOR& pos);

		std::string GetName() { return _name; }
	private:
		bool _ChannelIsNullptr();

		static FMODCore* _instance;

		FMOD::Sound* _sound = nullptr;
		FMOD::Channel* _channel = nullptr;
		float _volume = 0.0f;
		bool _is3D = false;

		std::string _name;
	};

	class DLL FMODCore {
		friend DLL CoreSound;
	public:
		FMODCore(int maxChannels = 512,
			const std::string& defaultPath = "./Assets/Sounds/",
			const Listener& listener = Listener());
		~FMODCore();

		void Update();
		int GetChannelsPlaying();

	private:
		FMOD::Sound* _CreateSound(std::string fileName, int flags = FMOD_2D);

		void _PlaySound(CoreSound& sound);

		Listener _listener;

		bool _IsChannelPlaying(FMOD::Channel* c);
		FMOD::Channel* _GetFreeChannel();

		void _ErrorCheck(FMOD_RESULT result, std::string errorAt);

		FMOD::System* _fmodSystem = nullptr;
		std::string _defaultPath;
		std::vector<FMOD::Channel*> _channels;
	};

	//////////////////////////////////////////
	//Studio
	//////////////////////////////////////////

	class DLL StudioSound {
		friend FMODStudio;
	public:
		StudioSound();

		void PlayEvent(const std::string& eventName);
		void StopEvent(const std::string& eventName);
		void SetEventParameter(
			const std::string& eventName,
			const std::string& parameterName,
			float paramValue);

		void SetEventPosition(const std::string& eventName,const FMOD_VECTOR& pos);
		void LoadEvent(const std::string& eventName);

		FMOD_VECTOR forward = { 0.0f,0.0f,1.0f };
		FMOD_VECTOR up = { 0.0f,1.0f,0.0f };
	private:

		FMOD_VECTOR _velocity;// not yet implemented

		static FMODStudio* _instance;

		std::unordered_map<std::string, FMOD::Studio::EventDescription*> _bankEventDescriptions;
		std::unordered_map<std::string, FMOD::Studio::EventInstance*> _bankEventInstances;

	};

	class DLL FMODStudio {
		friend StudioSound;
	public:
		FMODStudio(int maxChannels = 512,
			const std::string& defaultPath = "./Assets/Sounds/",
			const Listener& listener = Listener());
		~FMODStudio();

		void Update();
		void LoadBank(const std::string& bankName,
			FMOD_STUDIO_LOAD_BANK_FLAGS flags = FMOD_STUDIO_LOAD_BANK_NORMAL);
	private:

		FMOD::Studio::EventDescription* _GetEventDescription(const std::string& eventName);

		void _SetEventDescriptionCallback(FMOD::Studio::EventDescription* eventDesc,
			void* callback, FMOD_STUDIO_EVENT_CALLBACK_TYPE type);
		void _SetEventInstanceCallback(FMOD::Studio::EventInstance* eventInstance,
			void* callback);//Not yet implemented

		void _ErrorCheck(FMOD_RESULT result, std::string errorAt);

		Listener _listener;
		FMOD_3D_ATTRIBUTES _atr;

		FMOD::Studio::System* _fmodSystem = nullptr;
		std::string _defaultPath;

	};


}