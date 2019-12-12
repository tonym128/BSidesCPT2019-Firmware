#include "platform_core.h"
#ifdef ESP32
#else
	#include "SDL.h"
	#include "SDL_main.h"
#endif

// Platform Specific Code
#ifdef SDL2_FOUND
void gameSleep(int sleepMS) {
		SDL_Delay(sleepMS);
}

unsigned long getTimeInMillis() {
    return SDL_GetTicks();
}

#elif ESP32
void gameSleep(int sleepMS) {
	delay(sleepMS);
}

unsigned long getTimeInMillis() {
	return millis();
}
#endif
