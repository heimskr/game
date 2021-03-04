#include <stdexcept>

#include "Area.h"
#include "Region.h"

Area::Area(Region *parent_, size_t size_): parent(parent_), size(size_) {}

Area & Area::setResources(const Resource::Map &resources_) {
	resources = resources_;
	return *this;
}

Area & Area::setSize(size_t size_) {
	size = size_;
	return *this;
}

Area & Area::setPlayerOwned(bool player_owned) {
	playerOwned = player_owned;
	return *this;
}

Area & Area::setName(const std::string &name_) {
	if (name_.find_first_of(INVALID_CHARS) != std::string::npos)
		throw std::invalid_argument("Invalid area name");
	name = name_;
	return *this;
}

size_t Area::totalPopulation() const {
	size_t out = 0;
	for (const auto &pair: resources)
		if (Resource::hasType(*parent->owner, pair.first, "sapient"))
			out += static_cast<size_t>(pair.second);
	return out;
}
