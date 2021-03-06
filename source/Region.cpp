#include <sstream>

#include "Game.h"
#include "Region.h"
#include "Util.h"

Region::Region(Game *owner_, const std::string &name_, const Position &position_, size_t size_):
	owner(owner_), name(name_), position(position_), size(size_) {}

void Region::tick() {
	for (auto &pair: areas)
		pair.second->tick();
}

Resource::Map Region::allResources() const {
	Resource::Map out;
	for (const auto &area_pair: areas)
		for (const auto &resource_pair: area_pair.second->resources)
			out[resource_pair.first] += resource_pair.second;
	return out;
}

size_t Region::totalPopulation() const {
	// Hopefully nothing goes wrong with storing population as doubles.
	size_t out = 0;
	for (const auto &pair: areas)
		out += pair.second->totalPopulation();
	return out;
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
	out << name << ":" << position.first << ":" << position.second << ":" << size << ":" << money << ";";
	for (const auto &pair: areas)
		out << pair.second->toString() << ";";
	return out.str();
}

std::shared_ptr<Region> Region::fromString(Game &game, const std::string &str) {
	const std::vector<std::string> by_semicolon = split(str, ";", false);
	const std::vector<std::string> by_colon = split(by_semicolon[0], ":", false);
	const std::string &name = by_colon[0];
	const s64 x = parseLong(by_colon[1]);
	const s64 y = parseLong(by_colon[2]);
	const size_t size = parseUlong(by_colon[3]);
	const size_t money = parseUlong(by_colon[4]);
	std::shared_ptr<Region> region = std::make_shared<Region>(&game, name, std::make_pair(x, y), size);
	region->money = money;
	region->areas.clear();
	for (size_t i = 1; i < by_semicolon.size(); ++i) {
		if (by_semicolon[i].empty())
			continue;
		std::shared_ptr<Area> area = Area::fromString(*region, by_semicolon[i]);
		region->areas.emplace(area->name, area);
	}
	return region;
}
