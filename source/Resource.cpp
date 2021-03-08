#include "Game.h"
#include "Resource.h"

Resource::Resource(Game *owner_, const std::string &name_): owner(owner_), name(name_) {}
Resource::Resource(Game *owner_, const char *name_): owner(owner_), name(name_) {}

Resource & Resource::add(const std::string &name_, const Conversion &conversion) {
	conversions.emplace(name_, conversion);
	return *this;
}

bool Resource::hasType(const Type &type) const {
	return types.count(type) != 0;
}

bool Resource::hasType(Game &game, const std::string &name, const Type &type) {
	return game.resources.at(name).hasType(type);
}

Resource & Resource::setDiscrete(bool discrete_) {
	discrete = discrete_;
	return *this;
}

Resource & Resource::setDefaultExtractionRate(double rate) {
	defaultExtractionRate = rate;
	return *this;
}
