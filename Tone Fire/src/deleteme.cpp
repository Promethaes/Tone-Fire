#include "deleteme.h"
#include "FMOD/core/fmod.hpp"

int main() {

	FMOD::System* system = nullptr;
	FMOD::System_Create(&system);
	system->init(2, 0, 0);
	return 0;
}