#include "ToneFire.h"
#include <iostream>
#include <chrono>

int main() {
	//ToneFire::FMODCore fmod;
	//
	//ToneFire::CoreSound test("test.mp3", FMOD_2D);
	//ToneFire::CoreSound test2{ "test2.mp3",false,false,true };
	//
	//test.Play();
	//test2.Play();
	//
	//while (true) {
	//	fmod.Update();
	//}

	{
		ToneFire::FMODStudio fmod;
		fmod.LoadBank("Master.bank");
		fmod.LoadBank("Master.strings.bank");
		fmod.LoadBank("Ghoul.bank");

		ToneFire::StudioSound test;
		test.LoadEvent("event:/Ghoul/23 hurt");
		test.LoadEvent("event:/Ghoul/22 spotted");
		test.SetEventPosition("event:/Ghoul/23 hurt", FMOD_VECTOR{ 0.0f, 0.0f, 15.0f });
		//test.PlayEvent("event:/Ghoul/23 hurt");

		ToneFire::StudioSound test2;

		test2.PlayEvent("event:/Ghoul/23 hurt");
		while (true) {
			fmod.Update();

		}
	}
	system("pause");

	return 0;
}