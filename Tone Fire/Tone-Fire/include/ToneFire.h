#pragma once

//exports the stuff if we're building ToneFire, or imports it if we're using it
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


	/*
	* The Listener is something used by FMOD Core and Studio in order to have correct 3D sound.
	* ToneFire has an internal listener in both Core and Studio implementations. You do not need
	* to touch this.
	*/
	class DLL Listener {
		friend DLL FMODCore;
		friend DLL FMODStudio;
	public:
		Listener(
			FMOD_VECTOR pos = { 0.0f,0.0f,0.0f },
			FMOD_VECTOR up = { 0.0f,1.0f,0.0f },
			FMOD_VECTOR forward = { 0.0f,0.0f,1.0f }
		);

		FMOD_VECTOR& GetPosition();
		void SetPosition(FMOD_VECTOR pos);

	private:
		FMOD_VECTOR _position;
		FMOD_VECTOR _velocity;//Not yet implemented
		FMOD_VECTOR _up;
		FMOD_VECTOR _forward;
	};

	/*
	* This is a data type dedicated to making your life easier when it
	* comes to creating and playing sounds with FMOD Core.
	*/
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

	/*
	* This class is what handles all of the internal updating and such
	* of FMOD core. It needs to be updated once per frame, and should be
	* initialized along side something like the renderer. That is to say,
	* there should be FMODCore object, and it should be initialized before
	* you try to make any CoreSounds.
	*/
	class DLL FMODCore {
		friend DLL CoreSound;
	public:
		FMODCore(int maxChannels = 512,
			const std::string& defaultPath = "./Assets/Sounds/",
			const Listener& listener = Listener());
		~FMODCore();

		//update the underlying FMOD Core system. Must be called once per frame.
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

	/*
	* This is a studio sound, a class dedicated to making your life easier
	* when working with FMOD Studio events. It supports events, event parameters,
	* and even positioning the events in the world.
	*/
	class DLL StudioSound {
		friend FMODStudio;
	public:
		StudioSound();

		//Will play an event given the proper name.
		//If the event given is not loaded into memory, it will load that event.
		void PlayEvent(const std::string& eventName);

		//Will stop an event given the proper name.
		//If the event given is not loaded into memory, it will load that event.
		void StopEvent(const std::string& eventName);

		//Sets the parameter of a particular event.
		//If the event given is not loaded into memory, it will load that event.
		//Will throw an error if the parameter does not exist.
		void SetEventParameter(
			const std::string& eventName,
			const std::string& parameterName,
			float paramValue);

		//Will set the position of a particular event. NOTE THE FORWARD VECTOR BEING 0,0,1 BY DEFAULT!
		//If the event given is not loaded into memory, it will load that event.
		void SetEventPosition(const std::string& eventName,const FMOD_VECTOR& pos);

		//Loads a particular event into memory.
		void LoadEvent(const std::string& eventName);

		FMOD_VECTOR forward = { 0.0f,0.0f,1.0f };
		FMOD_VECTOR up = { 0.0f,1.0f,0.0f };
	private:

		FMOD_VECTOR _velocity;// not yet implemented

		static FMODStudio* _instance;

		std::unordered_map<std::string, FMOD::Studio::EventDescription*> _bankEventDescriptions;
		std::unordered_map<std::string, FMOD::Studio::EventInstance*> _bankEventInstances;

	};

	/*
	* This is the class that manages everything to do with FMOD Studio under the hood.
	* It should be initialized before any StudioSounds are created. It must be updated once
	* per frame. It is also how you will load Bank files into memory.
	*/
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