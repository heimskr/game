#include <sstream>

#include "Game.h"
#include "Region.h"

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
	out << name << ":" << position.first << ":" << position.second << ":" << size << ":" << money;
	// for (const auto 
	return out.str();
}
