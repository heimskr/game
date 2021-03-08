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
#include "UI.h"

// #define USE_NXLINK_STDIO

PadState pad;
std::vector<State> states {State::Initial};

void drawUI() {
	constexpr int SIZE = 4;
	fprintf(stderr, "%s:%d\n", __FILE__, __LINE__); fflush(stderr);
	rightPanel(console, 30);
	fprintf(stderr, "%s:%d\n", __FILE__, __LINE__); fflush(stderr);
	topPanel(console, SIZE);
	fprintf(stderr, "%s:%d\n", __FILE__, __LINE__); fflush(stderr);
	print("\e[43m\e[2J");
	fprintf(stderr, "%s:%d\n", __FILE__, __LINE__); fflush(stderr);
	bottomPanel(console, SIZE);
	fprintf(stderr, "%s:%d\n", __FILE__, __LINE__); fflush(stderr);
	print("\e[44m\e[2J");
	fprintf(stderr, "%s:%d\n", __FILE__, __LINE__); fflush(stderr);
	verticalPanel(console, SIZE, 45 - 2 * SIZE);
	fprintf(stderr, "%s:%d\n", __FILE__, __LINE__); fflush(stderr);
	print("\e[42m\e[2J");
	fprintf(stderr, "%s:%d\n", __FILE__, __LINE__); fflush(stderr);
	print("\e[0m");
	fprintf(stderr, "%s:%d\n", __FILE__, __LINE__); fflush(stderr);
	resetWindow(console);
	fprintf(stderr, "%s:%d\n", __FILE__, __LINE__); fflush(stderr);
}

int main(int argc, char *argv[]) {
	console = consoleGetDefault();
	consoleInit(console);
	socketInitializeDefault();
#ifdef USE_NXLINK_STDIO
	nxlinkStdio();
#else
	nxlinkConnectToHost(false, true);
#endif
	padConfigureInput(1, HidNpadStyleSet_NpadStandard);
	padInitializeDefault(&pad);
	hidInitializeTouchScreen();

	FS::init();

	srand(getTime());

	states.reserve(10);
	Context context;
	context.game = std::make_shared<Game>();

	std::function<void(const std::string &name)> chooseAction;

	const Action actions[] = {
		{"Add Region", State::Initial, [&] { context->addRegion(); }},
		{"Extract Resource", State::Initial, [&] { extractResource(context); }},
		{"List Extractions", State::Initial, [&] {
			if (context->extractions.empty()) {
				Logger::warn("No extractions are occurring.");
			} else {
				print("Extractions:\n");
				for (const Extraction &extraction: context->extractions)
					print("- \e[32m%s\e[0m from \e[36m%s\e[0m in \e[33m%s\e[0m @ \e[1m%.2f\e[22;2m/\e[22ms (\e[1m%.2f\e[22m left)\n",
						extraction.resourceName.c_str(), extraction.area->name.c_str(),
						extraction.area->parent->name.c_str(), extraction.rate, extraction.amount);
			}
		}},
		{"List Regions", State::Initial, [&] { context->listRegions(); }},
		{"List Region Resources", State::Initial, [&] { listRegionResources(context); }},
		{"Load", State::Initial, [&] {
			try {
				context.game = Game::load();
				print("Game loaded successfully.\n");
			} catch (const std::exception &err) {
				print("Couldn't load game: %s\n", err.what());
			}
		}},
		{"Load Defaults", State::Initial, [&] { context->loadDefaults(); chooseAction("List Regions"); }},
		{"Move", State::Initial, [&] { move(context); }},
		{"NameGen", State::Initial, [&] { printf("Name: %s\n", NameGen::makeRandomLanguage().makeName().c_str()); }},
		{"Rename Region", State::Initial, [&] { renameRegion(context); }},
		{"Save", State::Initial, [&] {
			try {
				context->save();
				Logger::success("Game saved successfully.");
			} catch (const std::exception &err) {
				Logger::error("Couldn't save game: %s", err.what());
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
		{"Teleport", State::Initial, [&] {
			selectRegion(context, [&](Region &region) {
				context->position = region.position;
				Logger::success("Moved to \e[33m%s\e[39m.", region.name.c_str());
			});
		}},
		{"Tick Once", State::Initial, [&] { context->tick(); }},
		{"Tick Many", State::Initial, [&] {
			Keyboard::openForNumber([&](s64 ticks) {
				for (s64 i = 0; i < ticks; ++i)
					context->tick();
				Logger::success("Ticked \e[1m%ld\e[22m times.\n", ticks);
			}, "Tick Count");
		}},
	};

	constexpr s32 actionCount = static_cast<s32>(sizeof(actions) / sizeof(actions[0]));
	s32 actionIndex = 0;

	auto displayAction = [&] {
		print("\e[999DSelect action: \e[33m%s\e[39m\e[0K", actions[actionIndex].name);
	};

	chooseAction = [&](const std::string &name) {
		for (s32 i = 0; i < actionCount; ++i)
			if (actions[i].name == name) {
				actionIndex = i;
				break;
			}
	};

	time_t last_time = getTime();

	HidTouchScreenState ts_state;

	while (appletMainLoop()) {
		padUpdate(&pad);
		u64 kDown = padGetButtonsDown(&pad);
		if (kDown & HidNpadButton_Plus)
			break;

		if (hidGetTouchScreenStates(&ts_state, 1) == 1) {
			for (int i = 0; i < ts_state.count; ++i)
				onTouch(ts_state.touches[i].x / 16, ts_state.touches[i].y / 16);
		}

		if (kDown & HidNpadButton_Y) {
			drawUI();
		}

		if (kDown & HidNpadButton_Minus) {
			clearScreen();
			continue;
		}

		const bool selectNext = (kDown & HidNpadButton_AnyDown) || (kDown & HidNpadButton_AnyRight);
		const bool selectPrev = (kDown & HidNpadButton_AnyUp)   || (kDown & HidNpadButton_AnyLeft);
		const bool aPressed = kDown & HidNpadButton_A;
		const bool bPressed = kDown & HidNpadButton_B;

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
				if (bPressed) {
					states.pop_back();
				} else if (aPressed) {
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
				if (bPressed) {
					states.pop_back();
				} else if (aPressed) {
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
				if (bPressed) {
					states.pop_back();
				} else if (aPressed) {
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
				if (!context.validDirections.empty()) {
					if (selectNext) {
						do {
							context.selectedDirection = static_cast<Direction>((static_cast<int>(context.selectedDirection) + 1) % 4);
						} while (context.validDirections.count(context.selectedDirection) == 0);
						changed = true;
					} else if (selectPrev) {
						do {
							if (context.selectedDirection == Direction::North)
								context.selectedDirection = Direction::West;
							else
								context.selectedDirection = static_cast<Direction>(static_cast<int>(context.selectedDirection) - 1);
						} while (context.validDirections.count(context.selectedDirection) == 0);
						changed = true;
					}
				}
				if (changed)
					displayDirection(context);
				if (bPressed) {
					states.pop_back();
				} else if (aPressed) {
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
	try {
		selectArea(context, context->currentRegion(), [&](Area &area) {
			if (!area.playerOwned) {
				Logger::error("You are not allowed to extract resources from that area.");
				return;
			}
			selectResource(context, area, [&](const std::string &resource, double amount) {
				print("Amount of \e[36m%s\e[39m available: \e[1m%f\e[22m\n", resource.c_str(), amount);
				svcSleepThread(1'000'000'000);
				if (!Keyboard::openForDouble([&](double chosen) {
					if (amount < chosen) {
						Logger::error("Not enough of that resource is available.");
					} else {
						context->extractions.emplace_back(&area, resource, chosen, context->resources.at(resource).defaultExtractionRate);
						// area.resources[resource] -= chosen;
						// context->inventory[resource] += chosen;
						Logger::success("Extracting \e[1m%f\e[22m x \e[36m%s\e[39m.", chosen, resource.c_str());
					}
				}, "Resource Amount")) {
					Logger::error("Invalid amount.");
				}
			});
		});
	} catch (const std::out_of_range &) {
		Logger::error("No region is selected.");
	}
}

void selectRegion(Context &context, std::function<void(Region &)> selectfn) {
	if (context->regions.empty()) {
		Logger::warn("No regions.");
	} else {
		context.regionIndex = 0;
		context.onRegionSelect = selectfn;
		displayRegion(context);
		states.push_back(State::SelectRegion);
	}
}

void selectArea(Context &context, Region &region, std::function<void(Area &)> selectfn) {
	if (region.areas.empty()) {
		Logger::warn("No areas.");
	} else {
		context.areaIndex = 0;
		context.onAreaSelect = selectfn;
		context.selectedRegion = &region;
		displayArea(context);
		states.push_back(State::SelectArea);
	}
}

void selectResource(Context &context, Area &area, std::function<void(const std::string &, double)> selectfn) {
	if (area.resources.empty()) {
		Logger::warn("No resources.");
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
	if (!context.validDirections.empty())
		while (context.validDirections.count(context.selectedDirection) == 0)
			context.selectedDirection = static_cast<Direction>((static_cast<int>(context.selectedDirection) + 1) % 4);
	context.onDirectionSelect = selectfn;
	displayDirection(context);
	states.push_back(State::SelectDirection);
}

void displayRegion(const Context &context) {
	// clearLine();
	print("\e[999DSelect region: \e[33m%s\e[39m\e[0K", std::next(context->regions.begin(), context.regionIndex)->second.name.c_str());
}

void displayArea(const Context &context) {
	if (!context.selectedRegion) {
		clearLine();
		Logger::warn("No region selected.");
		return;
	}
	print("\e[999DSelect area: \e[33m%s\e[39m\e[0K", std::next(context.selectedRegion->areas.begin(), context.areaIndex)->second->name.c_str());
}

void displayResource(const Context &context) {
	if (!context.selectedArea) {
		clearLine();
		Logger::warn("No area selected.");
		return;
	}
	print("\e[999DSelect resource: \e[33m%s\e[39m\e[0K", std::next(context.selectedArea->resources.begin(), context.resourceIndex)->first.c_str());
}

void displayDirection(const Context &context) {
	printf("\e[999DSelect direction: \e[33m%s\e[39m\e[0K", toString(context.selectedDirection));
}

void clearLine() {
	print("\e[2K\e[999D");
}

void clearScreen() {
	print("\e[2J");
}

void listRegionResources(Context &context) {
	Region *region;
	try {
		region = &context->currentRegion();
	} catch (const std::out_of_range &) {
		Logger::error("No region is selected.");
		return;
	}
	Resource::Map resources = region->allResources();
	if (resources.empty()) {
		printf("\e[33m%s\e[39m has no resources.\n", region->name.c_str());
	} else {
		printf("Resources for \e[33m%s\e[39m:\n", region->name.c_str());
		for (const auto &pair: resources)
			printf("- \e[32m%s\e[39m x \e[1m%f\e[22m\n", pair.first.c_str(), pair.second);
	}
}

void move(Context &context) {
	Region *region;
	try {
		Logger::info("You are at \e[33m%s\e[39m.", (region = &context->currentRegion())->name.c_str());
	} catch (const std::out_of_range &) {
		return;
	}
	if (!region->hasNeighbor()) {
		Logger::warn("No regions are adjacent.");
		return;
	}
	context.validDirections = region->validDirections();
	selectDirection(context, [&](Direction direction) {
		s64 offset_x = 0, offset_y = 0;
		switch (direction) {
			case Direction::North: offset_y = -1; break;
			case Direction::East:  offset_x =  1; break;
			case Direction::South: offset_y =  1; break;
			case Direction::West:  offset_x = -1; break;
			default: throw std::runtime_error("Invalid direction: " + std::to_string(static_cast<int>(direction)));
		}
		Region::Position new_position = {context->position.first + offset_x, context->position.second + offset_y};
		if (context->regions.count(new_position) == 0) {
			Logger::error("No region exists in that direction.");
		} else {
			context->position = new_position;
			Logger::success("Moved to \e[33m%s\e[39m.", context->regions.at(new_position).name.c_str());
		}
	});
}

void renameRegion(Context &context) {
	Region *region;
	try {
		region = &context->currentRegion();
	} catch (const std::exception &err) {
		Logger::error("Couldn't get current region.");
		return;
	}
	if (!Keyboard::openForText([&](std::string new_name) {
		if (new_name == region->name) {
			Logger::warn("Region name not updated.");
		} else {
			Logger::success("Renamed \e[33m%s\e[39m to \e[33m%s\e[39m.", region->name.c_str(), new_name.c_str());
			context->updateName(*region, new_name);
		}
	}, "New Region Name", "", 64, NameGen::makeRandomLanguage().makeName())) {
		Logger::error("Invalid name.");
	}
}
