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
	// auto clearScreen = [] { printf("\e[2J"); };

	std::function<void(const std::string &name)> chooseAction;
	std::function<void()> displayRegion;
	std::function<void(std::function<void()>)> selectRegion;

	std::function<void()> onRegionSelect = [] {};
	s32 regionIndex = 0;

	const Action actions[] = {
		{"Load Defaults", State::Initial, [&] { game.loadDefaults(); chooseAction("List Regions"); }},
		{"List Regions", State::Initial, [&] { game.listRegions(); }},
		{"List Region Resources", State::Initial, [&] { selectRegion([&] {
			Region &region = std::next(game.regions.begin(), regionIndex)->second;
			Resource::Map resources = region.allResources();
			if (resources.empty()) {
				print("\e[1m%s\e[22m has no resources.\n", region.name.c_str());
			} else {
				print("Resources for \e[1m%s\e[22m:\n", region.name.c_str());
				for (const auto &pair: resources)
					print("- \e[32m%s\e[39m x \e[1m%f\e[22m\n", pair.first.c_str(), pair.second);
			}
		}); }},
		{"Tick Once", State::Initial, [&] { game.tick(); }},
		{"Tick Many", State::Initial, [&] {
			Keyboard::openForNumber([&](s64 ticks) {
				for (s64 i = 0; i < ticks; ++i)
					game.tick();
				print("Ticked \e[1m%ld\e[22m times.\n", ticks);
			}, "Tick Count");
		}},
		{"Add Region", State::Initial, [&] { game.addRegion(); }},
		{"NameGen", State::Initial, [&] { printf("Name: %s\n", NameGen::makeRandomLanguage().makeName().c_str()); }},
	};

	constexpr s32 actionCount = static_cast<s32>(sizeof(actions) / sizeof(actions[0]));
	s32 actionIndex = 0;

	auto displayAction = [&] {
		clearLine();
		print("Select action: \e[33m%s\e[0m", actions[actionIndex].name);
	};

	selectRegion = [&](std::function<void()> selectfn) {
		if (game.regions.empty()) {
			print("No regions.\n");
		} else {
			regionIndex = 0;
			onRegionSelect = selectfn;
			displayRegion();
			state = State::SelectRegion;
		}
	};

	displayRegion = [&] {
		clearLine();
		print("Select region: \e[33m%s\e[0m", std::next(game.regions.begin(), regionIndex)->second.name.c_str());
	};

	chooseAction = [&](const std::string &name) {
		for (s32 i = 0; i < actionCount; ++i)
			if (actions[i].name == name) {
				actionIndex = i;
				break;
			}
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

			case State::SelectRegion: {
				if (game.regions.empty())
					break;
				bool changed = false;
				if (selectNext) {
					regionIndex = (regionIndex + 1) % game.regions.size();
					changed = true;
				} else if (selectPrev) {
					regionIndex = regionIndex == 0? game.regions.size() - 1 : regionIndex - 1;
					changed = true;
				}
				if (changed)
					displayRegion();
				if (aPressed) {
					clearLine();
					onRegionSelect();
					if (state == State::SelectRegion)
						state = State::Initial;
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
