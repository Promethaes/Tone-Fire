#include "ToneFire.h"
#include <iostream>

int main() {
	ToneFire::ToneFireFMOD fmod;

	ToneFire::Sound2D test{ "test.mp3",true };
	ToneFire::Sound2D test2{ "test2.mp3" };
	

	test.Play();
	test2.Play();



	while (true) {
		fmod.Update();
	}


	return 0;
}