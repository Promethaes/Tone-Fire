#include "FmodImplementation.h"

int main() {
	ToneFire::FMODImplementation fmod;

	ToneFire::Sound2D test{ "test.mp3" };

	test.Play();

	while (true);


	return 0;
}