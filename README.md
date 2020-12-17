# Tone Fire
A C++ Wrapper for both FMOD Core and FMOD Studio API, which makes it much easier to use.

## Some Info
An example of how to use the FMOD Core Implementation:
```cpp
int main(){
	ToneFire::FMODCore core;
	ToneFire::CoreSound _sound{ "test.mp3",true,true,false };
	_sound.Play();

    //Game Loop
	while (true) {
		core.Update();
	}
    return 0;
}
```
An example of how to use the FMOD Studio Implementation:
```cpp
int main(){
    ToneFire::FMODStudio studio;

	studio.LoadBank("Master.bank");
	studio.LoadBank("Master.strings.bank");
	studio.LoadBank("Ghoul.bank");

	ToneFire::StudioSound test;
	test.LoadEvent("event:/Ghoul/23 hurt");
	test.LoadEvent("event:/Ghoul/22 spotted");
	test.SetEventPosition("event:/Ghoul/23 hurt", FMOD_VECTOR{ 0.0f, 0.0f, 15.0f });
	test.PlayEvent("event:/Ghoul/23 hurt");

    //Game Loop
	while (true) {
		studio.Update();
	}
	return 0;
}
```
## Extra Info
Make sure to call Play on FMOD Core Sounds before attempting to edit the channel (eg. change position, volume), and also make sure you load all of the banks into the FMOD Studio implementation before you make any Studio Sounds. Lastly, since you will be using FMOD you need to comply to their license terms as well, one of which is to include their logo in the application you're making. Heres a link: https://www.fmod.com/download

Finally, don't have a Core and a Studio instance at the same time. FMOD Studio's underlying systems create a core system, and this could really mess with the backend.

## How To add to your C++ Project
Simply add the headers required (FMOD stuff,ToneFire.h) to your include directory, and statically link ToneFire.lib. Then, copy and paste fmod.dll, fmodstudio.dll, and ToneFire.dll into your exe directory, and you'll be good to go!
