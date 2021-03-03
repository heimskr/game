#include <sstream>
#include <string>
#include <utility>

#include "Game.h"
#include "Keyboard.h"
#include "Util.h"
#include "NameGen.h"

namespace Globals {
	bool done = false;
	std::map<Resource::Name, Resource> resources;
}

Game::Game() {
	addAll();
}

void Game::add(const Resource &resource) {
	resources.emplace(resource.name, resource);
}

void Game::addResources() {
	add(Resource(this, "charcoal").addTypes("fuel"));
	add(Resource(this, "coal").addTypes("fuel"));
	add(Resource(this, "wood").addTypes("fuel", "natural").add("charcoal", {1, {"furnace"}}));
	add(Resource(this, "water").addTypes("liquid"));
	add(Resource(this, "ironOre").addTypes("ore", "iron").add("iron", {0.5, {"furnace"}}));
	add(Resource(this, "iron").addTypes("metal", "iron"));
	add(Resource(this, "human").addTypes("alive", "sapient"));
}

void Game::addAll() {
	if (!ready) {
		addResources();
		ready = true;
	}
}

void Game::listRegions() {
	if (regions.empty()) {
		printf("No regions.\n");
	} else {
		printf("Regions:\n");
		for (const auto &pair: regions)
			printf("- \e[1m%s\e[22m at (%ld, %ld)\n", pair.second.name.c_str(), pair.first.first, pair.first.second);
	}

	consoleUpdate(nullptr);
}

bool Game::updatePosition(Region &region, const decltype(Region::position) &new_position) {
	if (regions.count(region.position) == 0)
		return false;
	auto handler = regions.extract(region.position);
	handler.mapped().position = new_position;
	if (&region != &handler.mapped())
		region.position = new_position;
	handler.key() = new_position;
	regions.insert(std::move(handler));
	return true;
}

Region * Game::addRegion() {
	std::string name;
	if (!Keyboard::openForText([&](std::string entered_name) {
		name = std::move(entered_name);
	}, "Region Name", "", 64, NameGen::makeRandomLanguage().makeName())) {
		print("Invalid name.\n");
		return nullptr;
	}
	for (const auto &pair: regions)
		if (pair.second.name == name) {
			print("A region with that name already exists.\n");
			return nullptr;
		}
	if (name.find_first_of(Region::INVALID_CHARS) != std::string::npos) {
		print("Invalid region name.\n");
		return nullptr;
	}
	s64 x, y;
	size_t size;
	if (!Keyboard::openForNumber([&](s64 x_) { x = x_; }, "Region X Coordinate", "", 64, "", "-")) {
		print("Invalid x coordinate.\n");
		return nullptr;
	}
	if (!Keyboard::openForNumber([&](s64 y_) { y = y_; }, "Region Y Coordinate", "", 64, "", "-")) {
		print("Invalid y coordinate.\n");
		return nullptr;
	}
	if (regions.count({x, y}) != 0) {
		print("A region already exists at (%ld, %ld).\n", x, y);
		return nullptr;
	}
	if (!Keyboard::openForNumber([&](s64 size_) { size = static_cast<size_t>(size_); }, "Region Size")) {
		print("Invalid size.\n");
		return nullptr;
	}
	regions.insert({{x, y}, Region(this, name, {x, y}, size)});
	print("Created new region \e[1m%s\e[22m at position (%ld, %ld) with size %lu.\n", name.c_str(), x, y, size);
	return &regions.at({x, y});
}

std::string Game::toString() const {
	std::stringstream out;
	out << "[Regions]\n";
	for (const auto &pair: regions)
		out << pair.first.first << "," << pair.first.second << "=" << pair.second.toString() << "\n";
	out << "\n";
	return out.str();
}
