#include <sstream>

#include "Game.h"
#include "NameGen.h"
#include "Region.h"
#include "Util.h"
#include "area/Areas.h"

Region::Region(Game *owner_, const std::string &name_, const Position &position_, size_t size_):
		owner(owner_), name(), position(position_), size(size_) {
	setName(name_);
}

void Region::tick(double delta) {
	for (auto &pair: areas)
		pair.second->tick(delta);
}

Resource::Map Region::allResources() const {
	Resource::Map out;
	for (const auto &area_pair: areas)
		for (const auto &resource_pair: area_pair.second->resources)
			out[resource_pair.first] += resource_pair.second;
	return out;
}

Resource::Map Region::allOwnedResources() const {
	Resource::Map out;
	for (const auto &area_pair: areas)
		if (area_pair.second->playerOwned)
			for (const auto &resource_pair: area_pair.second->resources)
				out[resource_pair.first] += resource_pair.second;
	return out;
}

Resource::Map Region::allNonOwnedResources() const {
	Resource::Map out;
	for (const auto &area_pair: areas)
		if (!area_pair.second->playerOwned)
			for (const auto &resource_pair: area_pair.second->resources)
				out[resource_pair.first] += resource_pair.second;
	return out;
}

void Region::subtractResourceFromNonOwned(const std::string &resource_name, double amount) {
	for (auto &pair: areas) {
		Area &area = *pair.second;
		if (!area.playerOwned && area.resources.count(resource_name) != 0) {
			double &area_amount = area.resources.at(resource_name);
			if (amount <= area_amount) {
				area_amount -= amount;
				if (area_amount < Resource::MIN_AMOUNT)
					area.resources.erase(resource_name);
				return;
			} else {
				area.resources.erase(resource_name);
				amount -= area_amount;
				shrink(area.resources, resource_name);
			}
		}
	}

	Logger::error("Amount of %s remaining: %f", resource_name.c_str(), amount);
	throw std::runtime_error("Couldn't fully subtract resource");
}

size_t Region::totalPopulation() const {
	// Hopefully nothing goes wrong with storing population as doubles.
	size_t out = 0;
	for (const auto &pair: areas)
		out += pair.second->totalPopulation();
	return out;
}

bool Region::occupied() const {
	for (const auto &pair: areas)
		if (pair.second->getType() == Area::Type::Housing)
			return true;
	return false;
}

std::shared_ptr<HousingArea> Region::getHousing() {
	for (const auto &pair: areas)
		if (pair.second->getType() == Area::Type::Housing)
			return std::static_pointer_cast<HousingArea>(pair.second); // No RTTI =(
	return nullptr;
}

std::shared_ptr<Area> Region::getArea(Area::Type type) {
	for (const auto &pair: areas)
		if (pair.second->getType() == type)
			return pair.second;
	return nullptr;
}

bool Region::hasNeighbor() const {
	return owner->regions.count(position + Position( 0, -1)) != 0
	    || owner->regions.count(position + Position( 1,  0)) != 0
	    || owner->regions.count(position + Position( 0,  1)) != 0
	    || owner->regions.count(position + Position(-1,  0)) != 0;
}

bool Region::hasNeighbor(Direction direction) const {
	const Position offset = getOffset(direction);
	return owner->regions.count(position + offset) != 0;
}

Region * Region::getNeighbor(Direction direction) const {
	const Position offset = getOffset(direction);
	return owner->regions.count(position + offset) == 0? nullptr : owner->regions.at(position + offset).get();
}

std::unordered_set<Direction> Region::validDirections() const {
	std::unordered_set<Direction> out;
	out.reserve(4);
	if (owner->regions.count(position + Position( 0, -1)) != 0)
		out.insert(Direction::North);
	if (owner->regions.count(position + Position( 1,  0)) != 0)
		out.insert(Direction::East);
	if (owner->regions.count(position + Position( 0,  1)) != 0)
		out.insert(Direction::South);
	if (owner->regions.count(position + Position(-1,  0)) != 0)
		out.insert(Direction::West);
	return out;
}

void Region::erase(Area &area) {
	if (areas.count(area.name) == 0)
		return;

	owner->eraseExtractions(area);
	areas.erase(area.name);
}

bool Region::updateName(Area &area, const std::string &new_name) {
	if (areas.count(area.name) == 0)
		return false;
	auto handler = areas.extract(area.name);
	handler.mapped()->name = new_name;
	if (&area != handler.mapped().get())
		area.name = new_name;
	handler.key() = new_name;
	areas.insert(std::move(handler));
	return true;
}

Region & Region::setSize(size_t size_) {
	size = size_;
	return *this;
}

Region & Region::setMoney(size_t money_) {
	money = money_;
	return *this;
}

Region & Region::setPosition(const std::pair<s64, s64> &position_) {
	owner->updatePosition(*this, position_);
	return *this;
}

Region & Region::setName(const std::string &name_) {
	if (name_.find_first_of(INVALID_CHARS) != std::string::npos)
		throw std::invalid_argument("Invalid region name");
	name = name_;
	return *this;
}

Region & Region::operator+=(std::shared_ptr<Area> area) {
	if (area->name.empty()) {
		size_t i;
		std::string last_name;
		for (i = 0; areas.count(last_name = "Area " + std::to_string(i)) != 0; ++i);
		area->name = last_name;
	} else if (areas.count(area->name) != 0) {
		size_t i;
		std::string last_name;
		for (i = 2; areas.count(last_name = area->name + " (" + std::to_string(i) + ")") != 0; ++i);
		area->name = last_name;
	}

	areas.emplace(area->name, area);
	return *this;
}

std::string Region::toString() const {
	std::stringstream out;
	out << name << ":" << position.first << ":" << position.second << ":" << size << ":" << money << ":" << greed << ";";
	for (const auto &pair: areas)
		out << pair.second->toString() << ";";
	return out.str();
}

std::unique_ptr<Region> Region::fromString(Game &game, const std::string &str) {
	const std::vector<std::string> by_semicolon = split(str, ";", false);
	const std::vector<std::string> by_colon = split(by_semicolon[0], ":", false);
	const std::string &name = by_colon[0];
	const s64 x = parseLong(by_colon[1]);
	const s64 y = parseLong(by_colon[2]);
	const size_t size = parseUlong(by_colon[3]);
	const size_t money = parseUlong(by_colon[4]);
	const double greed = parseDouble(by_colon[5]);
	std::unique_ptr<Region> region = std::make_unique<Region>(&game, name, std::make_pair(x, y), size);
	region->money = money;
	region->greed = greed;
	region->areas.clear();
	for (size_t i = 1; i < by_semicolon.size(); ++i) {
		if (by_semicolon[i].empty())
			continue;
		std::shared_ptr<Area> area = Area::fromString(*region, by_semicolon[i]);
		region->areas.emplace(area->name, area);
	}
	return region;
}

Region::Position operator+(const Region::Position &left, const Region::Position &right) {
	return {left.first + right.first, left.second + right.second};
}

std::unique_ptr<Region> Region::generate(Game &game, const Position &pos, size_t size) {
	NameGen::Language language = NameGen::makeRandomLanguage();
	std::unique_ptr<Region> region = std::make_unique<Region>(&game, language.makeName(), pos, size);
	region->money = randomRange(100ul, 10000ul);
	region->greed = randomRangeDouble(0.1, 1.0);
	size_t remaining_size = size;

	bool populated = false;

	if (chance(0.36)) {
		populated = true;
		const size_t housing_size = randomRange(std::min(5ul, remaining_size), remaining_size / 2);
		auto housing = std::make_shared<HousingArea>(region.get(), housing_size);
		housing->setName("Town").setPlayerOwned(false);
		*region += housing;
		remaining_size -= housing_size;
	}

	if (remaining_size && chance(0.8)) {
		const size_t forest_size = randomRange(std::min(4ul, remaining_size), remaining_size / 2);
		auto forest = std::make_shared<ForestArea>(region.get(), forest_size);
		const char *types[] = {"Forest", "Woods", "Weald"};
		forest->setName(language.makeName() + " " + types[randomRange(0ul, sizeof(types) / sizeof(types[0]) - 1)]);
		forest->setPlayerOwned(!populated);
		*region += forest;
		remaining_size -= forest_size;
	}

	if (remaining_size && chance(0.5)) {
		const size_t mountain_size = randomRange(std::min(5ul, remaining_size), remaining_size / 2);
		auto mountain = std::make_shared<MountainArea>(region.get(), mountain_size);
		mountain->setName("Mountain").setPlayerOwned(!populated);
		*region += mountain;
		remaining_size -= mountain_size;
	}

	if (remaining_size && chance(0.4)) {
		const size_t lake_size = randomRange(1, remaining_size / 4);
		auto lake = std::make_shared<LakeArea>(region.get(), lake_size);
		lake->setName("Lake").setPlayerOwned(!populated);
		*region += lake;
		remaining_size -= lake_size;
	}

	if (populated && remaining_size && chance(0.8)) {
		const size_t farmland_size = randomRange(1, remaining_size * 2 / 3);
		auto farmland = std::make_shared<FarmlandArea>(region.get(), farmland_size);
		farmland->setName("Farmland").setPlayerOwned(false);
		*region += farmland;
		remaining_size -= farmland_size;
	}

	if (remaining_size) {
		auto empty = std::make_shared<EmptyArea>(region.get(), remaining_size);
		empty->setName("Empty Land").setPlayerOwned(!populated);
		*region += empty;
	}

	return region;
}

std::unique_ptr<Region> Region::generate(Game &game, const Position &pos) {
	return generate(game, pos, randomRange(32, 128));
}
