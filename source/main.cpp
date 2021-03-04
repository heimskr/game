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
std::vector<State> states {State::Initial};

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

	states.reserve(10);
	Game game;

	auto clearLine = [] { printf("\e[2K\e[999D"); };
	auto clearScreen = [] { printf("\e[2J"); };

	std::function<void(const std::string &name)> chooseAction;
	std::function<void()> displayRegion, displayArea, displayResource;
	std::function<void(std::function<void(Region &)>)> selectRegion;
	std::function<void(Region &, std::function<void(Area &)>)> selectArea;
	std::function<void(Area &, std::function<void(const Resource::Name &, double)>)> selectResource;

	std::function<void(Region &)> onRegionSelect = [](Region &) {};
	std::function<void(Area &)> onAreaSelect = [](Area &) {};
	std::function<void(const Resource::Name &, double)> onResourceSelect = [](const Resource::Name &, double) {};
	s32 regionIndex = 0, areaIndex = 0, resourceIndex = 0;

	Region *selectedRegion = nullptr;
	Area *selectedArea = nullptr;
	Resource::Name selectedResource;

	const Action actions[] = {
		{"Add Region", State::Initial, [&] { game.addRegion(); }},
		{"Extract Resource", State::Initial, [&] {
			selectRegion([&](Region &region) {
				// print("Selected region: %s\n", region.name.c_str());
				selectArea(region, [&](Area &area) {
					// print("Selected area: %s\n", area.name.c_str());
					selectResource(area, [&](const Resource::Name &resource, double amount) {
						print("Amount of \e[36m%s\e[39m available: \e[1m%f\e[22m\n", resource.c_str(), amount);
						svcSleepThread(1'000'000'000);
						if (!Keyboard::openForDouble([&](double chosen) {
							if (amount < chosen) {
								print("Not enough of that resource is available.\n");
							} else {
								area.resources[resource] -= chosen;
								game.inventory[resource] += chosen;
								print("Extracted \e[1m%f\e[22m x \e[36m%s\e[39m.\n", chosen, resource.c_str());
							}
						}, "Resource Amount")) {
							print("Invalid amount.\n");
						}
					});
				});
			});
		}},
		{"List Regions", State::Initial, [&] { game.listRegions(); }},
		{"List Region Resources", State::Initial, [&] {
			selectRegion([&](Region &region) {
				Resource::Map resources = region.allResources();
				if (resources.empty()) {
					print("\e[1m%s\e[22m has no resources.\n", region.name.c_str());
				} else {
					print("Resources for \e[1m%s\e[22m:\n", region.name.c_str());
					for (const auto &pair: resources)
						print("- \e[32m%s\e[39m x \e[1m%f\e[22m\n", pair.first.c_str(), pair.second);
				}
			});
		}},
		{"Load Defaults", State::Initial, [&] { game.loadDefaults(); chooseAction("List Regions"); }},
		{"NameGen", State::Initial, [&] { printf("Name: %s\n", NameGen::makeRandomLanguage().makeName().c_str()); }},
		{"Tick Once", State::Initial, [&] { game.tick(); }},
		{"Tick Many", State::Initial, [&] {
			Keyboard::openForNumber([&](s64 ticks) {
				for (s64 i = 0; i < ticks; ++i)
					game.tick();
				print("Ticked \e[1m%ld\e[22m times.\n", ticks);
			}, "Tick Count");
		}},
	};

	constexpr s32 actionCount = static_cast<s32>(sizeof(actions) / sizeof(actions[0]));
	s32 actionIndex = 0;

	auto displayAction = [&] {
		clearLine();
		print("Select action: \e[33m%s\e[0m", actions[actionIndex].name);
	};

	selectRegion = [&](std::function<void(Region &)> selectfn) {
		if (game.regions.empty()) {
			print("No regions.\n");
		} else {
			regionIndex = 0;
			onRegionSelect = selectfn;
			displayRegion();
			states.push_back(State::SelectRegion);
		}
	};

	selectArea = [&](Region &region, std::function<void(Area &)> selectfn) {
		if (region.areas.empty()) {
			print("No areas.\n");
		} else {
			areaIndex = 0;
			onAreaSelect = selectfn;
			selectedRegion = &region;
			displayArea();
			states.push_back(State::SelectArea);
		}
	};

	selectResource = [&](Area &area, std::function<void(const Resource::Name &, double)> selectfn) {
		if (area.resources.empty()) {
			print("No resources.\n");
		} else {
			resourceIndex = 0;
			onResourceSelect = selectfn;
			selectedArea = &area;
			displayResource();
			states.push_back(State::SelectResource);
		}
	};

	displayRegion = [&] {
		clearLine();
		print("Select region: \e[33m%s\e[0m", std::next(game.regions.begin(), regionIndex)->second.name.c_str());
	};

	displayArea = [&] {
		clearLine();
		if (!selectedRegion) {
			print("No region selected.");
			return;
		}
		print("Select area: \e[33m%s\e[0m", std::next(selectedRegion->areas.begin(), areaIndex)->second->name.c_str());
	};

	displayResource = [&] {
		clearLine();
		if (!selectedArea) {
			print("No area selected.\n");
			return;
		}
		print("Select resource: \e[33m%s\e[0m", std::next(selectedArea->resources.begin(), resourceIndex)->first.c_str());
	};

	chooseAction = [&](const std::string &name) {
		for (s32 i = 0; i < actionCount; ++i)
			if (actions[i].name == name) {
				actionIndex = i;
				break;
			}
	};

	while (appletMainLoop()) {
		padUpdate(&pad);
		u64 kDown = padGetButtonsDown(&pad);
		if (kDown & HidNpadButton_Plus)
			break;

		if (kDown & HidNpadButton_Minus) {
			clearScreen();
			continue;
		}

		const bool selectNext = (kDown & HidNpadButton_AnyDown) || (kDown & HidNpadButton_AnyRight);
		const bool selectPrev = (kDown & HidNpadButton_AnyUp)   || (kDown & HidNpadButton_AnyLeft);
		const bool aPressed = kDown & HidNpadButton_A;

		switch (states.back()) {
			case State::Initial:
				displayAction();
				states.back() = State::SelectAction;
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
					states.back() = actions[actionIndex].state;
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
					selectedRegion = &std::next(game.regions.begin(), regionIndex)->second;
					states.pop_back();
					onRegionSelect(*selectedRegion);
				}
				break;
			}

			case State::SelectArea: {
				if (!selectedRegion || selectedRegion->areas.empty())
					break;
				bool changed = false;
				if (selectNext) {
					areaIndex = (areaIndex + 1) % selectedRegion->areas.size();
					changed = true;
				} else if (selectPrev) {
					areaIndex = areaIndex == 0? selectedRegion->areas.size() - 1 : areaIndex - 1;
					changed = true;
				}
				if (changed)
					displayArea();
				if (aPressed) {
					clearLine();
					selectedArea = std::next(selectedRegion->areas.begin(), areaIndex)->second.get();
					states.pop_back();
					onAreaSelect(*selectedArea);
				}
				break;
			}

			case State::SelectResource: {
				if (!selectedArea || selectedArea->resources.empty())
					break;
				bool changed = false;
				if (selectNext) {
					resourceIndex = (resourceIndex + 1) % selectedArea->resources.size();
					changed = true;
				} else if (selectPrev) {
					resourceIndex = resourceIndex == 0? selectedArea->resources.size() - 1 : resourceIndex - 1;
					changed = true;
				}
				if (changed)
					displayResource();
				if (aPressed) {
					clearLine();
					auto iter = std::next(selectedArea->resources.begin(), resourceIndex);
					selectedResource = iter->first;
					states.pop_back();
					onResourceSelect(selectedResource, iter->second);
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
