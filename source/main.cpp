#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>

#include "main.h"

// #define USE_NXLINK_STDIO

PadState pad;
State state = State::Initial;

int main(int argc, char *argv[]) {
	consoleInit(nullptr);
	socketInitializeDefault(); // Initialize sockets
	nxlinkConnectToHost(false, false);
#ifdef USE_NXLINK_STDIO
	nxlinkStdio(); // Redirect stdout and stderr over the network to nxlink
#endif
	padConfigureInput(1, HidNpadStyleSet_NpadStandard);
	padInitializeDefault(&pad);

	srand(getTime());

	// auto clearLine = [] { printf("\e[2K\e[999D"); };
	auto clearScreen = [] { printf("\e[2J"); };

	while (appletMainLoop()) {
		padUpdate(&pad);
		u64 kDown = padGetButtonsDown(&pad);
		if (kDown & HidNpadButton_Plus)
			break;

		if (kDown & HidNpadButton_ZR) {
			clearScreen();
		}

		if (kDown & HidNpadButton_A) {
			printf("\e[2J\e[1;1HHello.\n");
		}

		consoleUpdate(nullptr);
	}

	cleanup();
	return 0;
}

void cleanup() {
	static bool cleaned = false;

	if (!cleaned) {
		socketExit();
		consoleExit(nullptr);
		cleaned = true;
	}
}

void perish() {
	consoleUpdate(nullptr);
	while (appletMainLoop()) {
		padUpdate(&pad);
		u64 kDown = padGetButtonsDown(&pad);
		if (kDown & HidNpadButton_Plus)
			break;
	}
	cleanup();
	exit(EXIT_FAILURE);
}

time_t getTime() {
	time_t current_time;
	Result result = timeGetCurrentTime(TimeType_UserSystemClock, (u64 *) &current_time);
	if (R_FAILED(result)) {
		printf("Couldn't get current time.\n");
		perish();
	}
	return current_time;
}