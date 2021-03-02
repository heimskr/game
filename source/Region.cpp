#include "Game.h"
#include "Region.h"

Region::Region(size_t size_): size(size_) {}

Resource::Map Region::allResources() const {
	Resource::Map out;
	for (const auto &area_pair: areas)
		for (const auto &resource_pair: area_pair.second.resources)
			out[resource_pair.first] += resource_pair.second;
	return out;
}

size_t Region::totalPopulation() const {
	// Hopefully nothing goes wrong with storing population as doubles.
	size_t out = 0;
	for (const auto &pair: areas)
		out += pair.second.totalPopulation();
	return out;
}

Region & Region::operator+=(Area area) {
	if (area.name.empty()) {
		size_t i;
		std::string last_name;
		for (i = 0; areas.count(last_name = "Area " + std::to_string(i)) != 0; ++i);
		area.name = last_name;
	} else if (areas.count(area.name) != 0) {
		size_t i;
		std::string last_name;
		for (i = 2; areas.count(last_name = area.name + " (" + std::to_string(i) + ")") != 0; ++i);
		area.name = last_name;
	}

	areas.emplace(area.name, area);
	return *this;
}
