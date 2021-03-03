#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>

#include "main.h"

#include "Resource.h"
#include "Region.h"
#include "Game.h"
#include "Util.h"
#include "Keyboard.h"
#include "NameGen.h"

// #define USE_NXLINK_STDIO

PadState pad;
State state = State::Initial;

int main(int argc, char *argv[]) {
	console = consoleGetDefault();
	consoleInit(console);
	socketInitializeDefault(); // Initialize sockets
#ifdef USE_NXLINK_STDIO
	nxlinkStdio(); // Redirect stdout and stderr over the network to nxlink
#else
	nxlinkConnectToHost(false, false);
#endif
	padConfigureInput(1, HidNpadStyleSet_NpadStandard);
	padInitializeDefault(&pad);

	srand(getTime());

	Game game;

	auto clearLine = [] { printf("\e[2K\e[999D"); };
	auto clearScreen = [] { printf("\e[2J"); };

	const Action actions[] = {
		{"List Regions", State::Initial, [&] { game.listRegions(); }},
		{"Add Region", State::Initial, [&] { game.addRegion(); }},
		{"NameGen", State::Initial, [&] { printf("Name: %s\n", NameGen::makeRandomLanguage().makeName().c_str()); }},
		// {"Import Mii", State::SelectMiiFromSD},
		// {"Export Mii", State::SelectMiiFromDB, displayDBMii},
	};

	constexpr size_t actionCount = sizeof(actions) / sizeof(actions[0]);
	s32 actionIndex = 0;

	auto displayAction = [&] {
		clearLine();
		print("Select Action: \e[33m%s\e[0m", actions[actionIndex].name);
	};

	if (console)
		print("(%d, %d)\n", console->consoleWidth, console->consoleHeight);

	while (appletMainLoop()) {
		padUpdate(&pad);
		u64 kDown = padGetButtonsDown(&pad);
		if (kDown & HidNpadButton_Plus)
			break;

		const bool selectNext = (kDown & HidNpadButton_AnyDown) || (kDown & HidNpadButton_AnyRight);
		const bool selectPrev = (kDown & HidNpadButton_AnyUp)   || (kDown & HidNpadButton_AnyLeft);
		const bool aPressed = kDown & HidNpadButton_A;

		switch (state) {
			case State::Initial:
				displayAction();
				state = State::SelectAction;
				break;

			case State::SelectAction: {
				bool changed = false;
				if (selectNext) {
					actionIndex = (actionIndex + 1) % actionCount;
					changed = true;
				} else if (selectPrev) {
					actionIndex = actionIndex == 0? actionCount - 1 : actionIndex - 1;
					changed = true;
				}
				if (changed)
					displayAction();
				if (aPressed) {
					clearLine();
					state = actions[actionIndex].state;
					actions[actionIndex].onSelect();
				}
				break;
			}

			default:
				break;
		}

		consoleUpdate(console);
	}

	cleanup();
	return 0;
}

void cleanup() {
	static bool cleaned = false;

	if (!cleaned) {
		socketExit();
		consoleExit(console);
		cleaned = true;
	}
}

void perish() {
	consoleUpdate(console);
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

std::string randomString(size_t length) {
	std::string out;
	out.reserve(length);
	for (size_t i = 0; i < length; ++i)
		out += (i == 0? 'A' : 'a') + (rand() % 26);
	return out;
}
