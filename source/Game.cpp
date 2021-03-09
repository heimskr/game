#include <sstream>
#include <string>
#include <utility>
#include <switch.h>

#include "FS.h"
#include "Game.h"
#include "Keyboard.h"
#include "NameGen.h"
#include "Util.h"
#include "area/Areas.h"

Game::Game() {
	addAll();
}

void Game::add(const Resource &resource) {
	resources.emplace(resource.name, resource);
}

void Game::addResources() {
	add(Resource(this, "charcoal").addTypes("fuel"));
	add(Resource(this, "coal").addTypes("fuel").setDefaultExtractionRate(0.1));
	add(Resource(this, "wood").addTypes("fuel", "natural").add("charcoal", {1, {"furnace"}}));
	add(Resource(this, "water").addTypes("liquid"));
	add(Resource(this, "ironOre").addTypes("ore", "iron").add("iron", {0.5, {"furnace"}}).setDefaultExtractionRate(0.1));
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
		print("No regions.\n");
	} else {
		Region *current = nullptr;
		try {
			current = &currentRegion();
		} catch (const std::exception &) {}
		print("Regions:\n");
		for (const auto &pair: regions) {
			if (pair.second.get() == current)
				print("- \e[33;4m%s\e[39;24m at (%ld, %ld)\n", pair.second->name.c_str(), pair.first.first, pair.first.second);
			else
				print("- \e[36m%s\e[39m at (%ld, %ld)\n", pair.second->name.c_str(), pair.first.first, pair.first.second);
			for (const auto &area_pair: pair.second->areas) {
				const Area &area = *area_pair.second;
				print("  - \e[35m%s\e[39m (%lu%s): %s\n",
					area_pair.first.c_str(), area.size, area.playerOwned? ", owned" : "", area.description().c_str());
			}
		}
	}
}

bool Game::updatePosition(Region &region, const Region::Position &new_position) {
	if (regions.count(region.position) == 0)
		return false;
	auto handler = regions.extract(region.position);
	handler.mapped()->position = new_position;
	if (&region != handler.mapped().get())
		region.position = new_position;
	handler.key() = new_position;
	regions.insert(std::move(handler));
	return true;
}

Region::Position Game::suggestPosition() {
	s64 x = 0, y = 0;
	enum Direction {Up, Right, Down, Left};
	Direction direction = Up;
	int max_extent = 1;
	int extent = 0;
	for (;;) {
		if (regions.count({x, y}) == 0)
			return {x, y};

		switch (direction) {
			case Up:    --y; break;
			case Right: ++x; break;
			case Down:  ++y; break;
			case Left:  --x; break;
		}

		if (++extent == max_extent) {
			extent = 0;
			switch (direction) {
				case Up:    direction = Right; break;
				case Right: direction = Down; ++max_extent; break;
				case Down:  direction = Left;  break;
				case Left:  direction = Up;   ++max_extent; break;
			}
		}
	}
}

bool Game::updateName(Region &region, const std::string &new_name) {
	region.name = new_name;
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
		if (pair.second->name == name) {
			print("A region with that name already exists.\n");
			return nullptr;
		}
	if (name.find_first_of(Region::INVALID_CHARS) != std::string::npos) {
		print("Invalid region name.\n");
		return nullptr;
	}
	s64 x, y;
	std::tie(x, y) = suggestPosition();
	size_t size;
	if (!Keyboard::openForNumber([&](s64 x_) { x = x_; }, "Region X Coordinate", "", 64, std::to_string(x), "-")) {
		print("Invalid x coordinate.\n");
		return nullptr;
	}
	if (!Keyboard::openForNumber([&](s64 y_) { y = y_; }, "Region Y Coordinate", "", 64, std::to_string(y), "-")) {
		print("Invalid y coordinate.\n");
		return nullptr;
	}
	if (regions.count({x, y}) != 0) {
		print("A region already exists at (%ld, %ld).\n", x, y);
		return nullptr;
	}
	if (!Keyboard::openForNumber([&](s64 size_) { size = static_cast<size_t>(size_); }, "Region Size", "", 64, std::to_string((rand() % 64) + 32))) {
		print("Invalid size.\n");
		return nullptr;
	}
	regions.insert({{x, y}, std::make_unique<Region>(this, name, Region::Position(x, y), size)});
	print("Created new region \e[1m%s\e[22m at position (%ld, %ld) with size %lu.\n", name.c_str(), x, y, size);
	return regions.at({x, y}).get();
}

Region & Game::currentRegion() {
	return *regions.at(position);
}

void Game::tick() {
	for (auto &pair: regions)
		pair.second->tick();
	for (auto iter = extractions.begin(); iter != extractions.end();) {
		Extraction &extraction = *iter;
		double to_extract = std::min(extraction.rate, extraction.amount);
		if (extraction.area->resources[extraction.resourceName] <= to_extract) {
			inventory[extraction.resourceName] += extraction.area->resources[extraction.resourceName];
			extraction.area->resources.erase(extraction.resourceName);
			extractions.erase(iter++);
		} else {
			inventory[extraction.resourceName] += to_extract;
			extraction.area->resources[extraction.resourceName] -= to_extract;
			extraction.amount -= to_extract;
			if (extraction.amount < 0.0001)
				extractions.erase(iter++);
			else
				++iter;
		}
	}
}

void Game::loadDefaults() {
	regions.clear();
	Region &home = *regions.insert({{0, 0}, std::make_unique<Region>(this, NameGen::makeRandomLanguage().makeName(), Region::Position(0, 0), 64)}).first->second;
	auto forest = std::make_shared<ForestArea>(&home, 20);
	auto housing = std::make_shared<HousingArea>(&home, 16);
	auto mountain = std::make_shared<MountainArea>(&home, 24);
	auto lake = std::make_shared<LakeArea>(&home, 4);
	forest->setName("Forest").setPlayerOwned(true);
	housing->setName("Small Town");
	mountain->setName("Mountain").setPlayerOwned(true);
	lake->setName("Lake").setPlayerOwned(true);
	home += forest;
	home += housing;
	home += mountain;
	home += lake;
	print("Loaded default data.\n");
}

std::string Game::toString() const {
	std::stringstream out;
	out << "[Regions]\n";
	for (const auto &pair: regions)
		out << pair.second->toString() << "\n";
	out << "\n[Inventory]\n";
	for (const auto &pair: inventory)
		out << pair.first << "=" << pair.second << "\n";
	out << "\n[Position]\n";
	out << position.first << "," << position.second << "\n";
	out << "\n[Extractions]\n";
	for (const Extraction &extraction: extractions)
		out << extraction.toString() << "\n";
	return out.str();
}

std::shared_ptr<Game> Game::fromString(const std::string &str) {
	std::vector<std::string> lines = split(str, "\n", true);
	enum class Mode {None, Regions, Inventory, Position, Extractions};
	Mode mode = Mode::None;

	std::shared_ptr<Game> out = std::make_shared<Game>();

	for (std::string &line: lines) {
		if (line.empty() || line == "\r")
			continue;
		if (line.back() == '\r')
			line.pop_back();
		if (line[0] == '[') {
			if (line == "[Regions]")
				mode = Mode::Regions;
			else if (line == "[Inventory]")
				mode = Mode::Inventory;
			else if (line == "[Position]")
				mode = Mode::Position;
			else if (line == "[Extractions]")
				mode = Mode::Extractions;
			else {
				Logger::error("Invalid line: \"%s\"", line.c_str());
				for (char ch: line)
					Logger::info("%d / '%c'", ch, ch);
				throw std::invalid_argument("Invalid line");
			}
		} else {
			switch (mode) {
				case Mode::Regions: {
					auto region = Region::fromString(*out, line);
					out->regions.emplace(region->position, std::move(region));
					break;
				}
				case Mode::Inventory: {
					const size_t equals = line.find('=');
					if (equals == std::string::npos)
						throw std::invalid_argument("Invalid Inventory line");
					out->inventory.emplace(line.substr(0, equals), parseDouble(line.substr(equals + 1)));
					break;
				}
				case Mode::Position: {
					const size_t comma = line.find(',');
					if (comma == std::string::npos)
						throw std::invalid_argument("Invalid Position line");
					out->position = {parseLong(line.substr(0, comma)), parseLong(line.substr(comma + 1))};
					break;
				}
				case Mode::Extractions: {
					out->extractions.push_back(Extraction::fromString(*out, line));
					break;
				}
				default: throw std::runtime_error("Invalid mode: " + std::to_string(static_cast<int>(mode)));
			}
		}
	}

	return out;
}

std::shared_ptr<Game> Game::load() {
	if (!FS::fileExists("/switch/TradeGame/save.txt"))
		throw std::runtime_error("Save data doesn't exist");
	return fromString(FS::readFile("/switch/TradeGame/save.txt"));
}

void Game::save() {
	Result result = 0;
	if (!FS::dirExists("/switch")) {
		if (R_FAILED(result = fsFsCreateDirectory(&fs, "/switch")))
			throw std::runtime_error("fsFsCreateDirectory(/switch) failed: 0x" + hex(result));
	}

	if (!FS::dirExists("/switch/TradeGame")) {
		if (R_FAILED(result = fsFsCreateDirectory(&fs, "/switch/TradeGame")))
			throw std::runtime_error("fsFsCreateDirectory(/switch/TradeGame) failed: 0x" + hex(result));
	}

	FS::writeFile("/switch/TradeGame/save.txt", toString());
}

Game & Game::operator+=(std::unique_ptr<Region> &&ptr) {
	if (regions.count(ptr->position) != 0)
		throw std::runtime_error("A region already exists at (" + std::to_string(ptr->position.first) + ", "
			+ std::to_string(ptr->position.first) + ")");
	regions.emplace(ptr->position, std::move(ptr));
	return *this;
}

Extraction * Game::getExtraction(const Area &area, const std::string &name) {
	for (Extraction &extraction: extractions)
		if (extraction.area == &area && extraction.resourceName == name)
			return &extraction;
	return nullptr;
}

const Extraction * Game::getExtraction(const Area &area, const std::string &name) const {
	for (const Extraction &extraction: extractions)
		if (extraction.area == &area && extraction.resourceName == name)
			return &extraction;
	return nullptr;
}
