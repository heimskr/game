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
#include "FS.h"

// #define USE_NXLINK_STDIO

PadState pad;
std::vector<State> states {State::Initial};

int main(int argc, char *argv[]) {
	console = consoleGetDefault();
	consoleInit(console);
	socketInitializeDefault();
#ifdef USE_NXLINK_STDIO
	nxlinkStdio();
#else
	nxlinkConnectToHost(false, false);
#endif
	padConfigureInput(1, HidNpadStyleSet_NpadStandard);
	padInitializeDefault(&pad);

	FS::init();

	srand(getTime());

	states.reserve(10);
	Context context;
	context.game = std::make_shared<Game>();

	std::function<void(const std::string &name)> chooseAction;

	const Action actions[] = {
		{"Add Region", State::Initial, [&] { context->addRegion(); }},
		{"Extract Resource", State::Initial, [&] { extractResource(context); }},
		{"List Regions", State::Initial, [&] { context->listRegions(); }},
		{"List Region Resources", State::Initial, [&] {
			selectRegion(context, [&](Region &region) {
				Resource::Map resources = region.allResources();
				if (resources.empty()) {
					printf("\e[1m%s\e[22m has no resources.\n", region.name.c_str());
				} else {
					printf("Resources for \e[1m%s\e[22m:\n", region.name.c_str());
					for (const auto &pair: resources)
						printf("- \e[32m%s\e[39m x \e[1m%f\e[22m\n", pair.first.c_str(), pair.second);
				}
			});
		}},
		{"Load", State::Initial, [&] {
			try {
				context.game = Game::load();
				print("Game loaded successfully.\n");
			} catch (const std::exception &err) {
				print("Couldn't load game: %s\n", err.what());
			}
		}},
		{"Load Defaults", State::Initial, [&] { context->loadDefaults(); chooseAction("List Regions"); }},
		{"Move", State::Initial, [&] {
			selectDirection(context, [&](Direction direction) {
				print("Direction: %d\n", direction);
			});
		}},
		{"NameGen", State::Initial, [&] { printf("Name: %s\n", NameGen::makeRandomLanguage().makeName().c_str()); }},
		{"Save", State::Initial, [&] {
			try {
				context->save();
				print("Game saved successfully.\n");
			} catch (const std::exception &err) {
				print("Couldn't save game: %s\n", err.what());
			}
		}},
		{"Show Inventory", State::Initial, [&] {
			if (context->inventory.empty())
				printf("No resources in inventory.\n");
			else
				for (const auto &pair: context->inventory)
					printf("- \e[36m%s\e[39m x \e[1m%f\e[22m\n", pair.first.c_str(), pair.second);
		}},
		{"Stringify", State::Initial, [&] { print("%s\n", context->toString().c_str()); }},
		{"Tick Once", State::Initial, [&] { context->tick(); }},
		{"Tick Many", State::Initial, [&] {
			Keyboard::openForNumber([&](s64 ticks) {
				for (s64 i = 0; i < ticks; ++i)
					context->tick();
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

	chooseAction = [&](const std::string &name) {
		for (s32 i = 0; i < actionCount; ++i)
			if (actions[i].name == name) {
				actionIndex = i;
				break;
			}
	};

	time_t last_time = getTime();

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
				if (context->regions.empty())
					break;
				bool changed = false;
				if (selectNext) {
					context.regionIndex = (context.regionIndex + 1) % context->regions.size();
					changed = true;
				} else if (selectPrev) {
					context.regionIndex = context.regionIndex == 0? context->regions.size() - 1 : context.regionIndex - 1;
					changed = true;
				}
				if (changed)
					displayRegion(context);
				if (aPressed) {
					clearLine();
					context.selectedRegion = &std::next(context->regions.begin(), context.regionIndex)->second;
					states.pop_back();
					context.onRegionSelect(*context.selectedRegion);
				}
				break;
			}

			case State::SelectArea: {
				if (!context.selectedRegion || context.selectedRegion->areas.empty())
					break;
				bool changed = false;
				if (selectNext) {
					context.areaIndex = (context.areaIndex + 1) % context.selectedRegion->areas.size();
					changed = true;
				} else if (selectPrev) {
					context.areaIndex = context.areaIndex == 0? context.selectedRegion->areas.size() - 1 : context.areaIndex - 1;
					changed = true;
				}
				if (changed)
					displayArea(context);
				if (aPressed) {
					clearLine();
					context.selectedArea = std::next(context.selectedRegion->areas.begin(), context.areaIndex)->second.get();
					states.pop_back();
					context.onAreaSelect(*context.selectedArea);
				}
				break;
			}

			case State::SelectResource: {
				if (!context.selectedArea || context.selectedArea->resources.empty())
					break;
				bool changed = false;
				if (selectNext) {
					context.resourceIndex = (context.resourceIndex + 1) % context.selectedArea->resources.size();
					changed = true;
				} else if (selectPrev) {
					context.resourceIndex = context.resourceIndex == 0? context.selectedArea->resources.size() - 1 : context.resourceIndex - 1;
					changed = true;
				}
				if (changed)
					displayResource(context);
				if (aPressed) {
					clearLine();
					auto iter = std::next(context.selectedArea->resources.begin(), context.resourceIndex);
					context.selectedResource = iter->first;
					states.pop_back();
					context.onResourceSelect(context.selectedResource, iter->second);
				}
				break;
			}

			case State::SelectDirection: {
				bool changed = false;
				if (selectNext) {
					context.selectedDirection = static_cast<Direction>((static_cast<int>(context.selectedDirection) + 1) % 4);
					changed = true;
				} else if (selectPrev) {
					if (context.selectedDirection == Direction::North)
						context.selectedDirection = Direction::West;
					else
						context.selectedDirection = static_cast<Direction>(static_cast<int>(context.selectedDirection) - 1);
					changed = true;
				}
				if (changed)
					displayDirection(context);
				if (aPressed) {
					clearLine();
					states.pop_back();
					context.onDirectionSelect(context.selectedDirection);
				}
				break;
			}

			default:
				break;
		}

		time_t new_time = getTime();
		for (s64 i = 0; i < new_time - last_time; ++i)
			context->tick();
		last_time = new_time;
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

void extractResource(Context &context) {
	selectRegion(context, [&](Region &region) {
		selectArea(context, region, [&](Area &area) {
			if (!area.playerOwned) {
				print("You are not allowed to extract resources from that area.\n");
				return;
			}
			selectResource(context, area, [&](const Resource::Name &resource, double amount) {
				print("Amount of \e[36m%s\e[39m available: \e[1m%f\e[22m\n", resource.c_str(), amount);
				svcSleepThread(1'000'000'000);
				if (!Keyboard::openForDouble([&](double chosen) {
					if (amount < chosen) {
						print("Not enough of that resource is available.\n");
					} else {
						area.resources[resource] -= chosen;
						context->inventory[resource] += chosen;
						print("Extracted \e[1m%f\e[22m x \e[36m%s\e[39m.\n", chosen, resource.c_str());
					}
				}, "Resource Amount")) {
					print("Invalid amount.\n");
				}
			});
		});
	});
}

void selectRegion(Context &context, std::function<void(Region &)> selectfn) {
	if (context->regions.empty()) {
		print("No regions.\n");
	} else {
		context.regionIndex = 0;
		context.onRegionSelect = selectfn;
		displayRegion(context);
		states.push_back(State::SelectRegion);
	}
}

void selectArea(Context &context, Region &region, std::function<void(Area &)> selectfn) {
	if (region.areas.empty()) {
		print("No areas.\n");
	} else {
		context.areaIndex = 0;
		context.onAreaSelect = selectfn;
		context.selectedRegion = &region;
		displayArea(context);
		states.push_back(State::SelectArea);
	}
}

void selectResource(Context &context, Area &area, std::function<void(const Resource::Name &, double)> selectfn) {
	if (area.resources.empty()) {
		print("No resources.\n");
	} else {
		context.resourceIndex = 0;
		context.onResourceSelect = selectfn;
		context.selectedArea = &area;
		displayResource(context);
		states.push_back(State::SelectResource);
	}
}

void selectDirection(Context &context, std::function<void(Direction)> selectfn) {
	context.selectedDirection = Direction::North;
	context.onDirectionSelect = selectfn;
	displayDirection(context);
	states.push_back(State::SelectDirection);
}

void displayRegion(const Context &context) {
	clearLine();
	print("Select region: \e[33m%s\e[0m", std::next(context->regions.begin(), context.regionIndex)->second.name.c_str());
}

void displayArea(const Context &context) {
	clearLine();
	if (!context.selectedRegion) {
		print("No region selected.");
		return;
	}
	print("Select area: \e[33m%s\e[0m", std::next(context.selectedRegion->areas.begin(), context.areaIndex)->second->name.c_str());
}

void displayResource(const Context &context) {
	clearLine();
	if (!context.selectedArea) {
		print("No area selected.\n");
		return;
	}
	print("Select resource: \e[33m%s\e[0m", std::next(context.selectedArea->resources.begin(), context.resourceIndex)->first.c_str());
}

void displayDirection(const Context &context) {
	clearLine();
	const char *dir;
	switch (context.selectedDirection) {
		case Direction::North: dir = "North"; break;
		case Direction::East:  dir = "East";  break;
		case Direction::South: dir = "South"; break;
		case Direction::West:  dir = "West";  break;
		default: dir = "???";
	}
	printf("Select direction: \e[33m%s\e[0m", dir);
}

void clearLine() {
	print("\e[2K\e[999D");
}

void clearScreen() {
	print("\e[2J");
}
