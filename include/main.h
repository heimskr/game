#pragma once

#include <functional>
#include <memory>
#include <string>
#include <sys/types.h>
#include <unordered_set>

#include "Direction.h"
#include "Resource.h"
#include "Util.h"

enum class State {Invalid, Initial, SelectAction, SelectRegion, SelectArea, SelectResource, SelectDirection, Extract};

class Area;
class Game;
class Region;

struct Action {
	const char *name;
	State state;
	std::function<void()> onSelect;

	Action(const char *name_, State state_):
		name(name_), state(state_), onSelect([] {}) {}

	Action(const char *name_, State state_, std::function<void()> onSelect_):
		name(name_), state(state_), onSelect(std::move(onSelect_)) {}
};

struct Context {
	std::shared_ptr<Game> game;
	s32 regionIndex = 0;
	s32 areaIndex = 0;
	s32 resourceIndex = 0;
	Region *selectedRegion = nullptr;
	Area *selectedArea = nullptr;
	Resource::Name selectedResource;
	Direction selectedDirection = Direction::North;
	std::unordered_set<Direction> validDirections = {Direction::North, Direction::East, Direction::South, Direction::West};
	std::function<void(Region &)> onRegionSelect = [](Region &) {};
	std::function<void(Area &)> onAreaSelect = [](Area &) {};
	std::function<void(const Resource::Name &, double)> onResourceSelect = [](const Resource::Name &, double) {};
	std::function<void(Direction)> onDirectionSelect = [](Direction) {};
	Game * operator->() const {
		return game.get();
	}
	void resetValidDirections() {
		validDirections = {Direction::North, Direction::East, Direction::South, Direction::West};
	}
};

void perish();
void cleanup();
time_t getTime();
std::string randomString(size_t);
void extractResource(Context &);
void selectRegion(Context &, std::function<void(Region &)>);
void selectArea(Context &, Region &, std::function<void(Area &)>);
void selectResource(Context &, Area &, std::function<void(const Resource::Name &, double)>);
void selectDirection(Context &, std::function<void(Direction)>);
void displayRegion(const Context &);
void displayArea(const Context &);
void displayResource(const Context &);
void displayDirection(const Context &);
void clearLine();
void clearScreen();
void listRegionResources(Context &);
void move(Context &);
