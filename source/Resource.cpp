#include "Game.h"
#include "Resource.h"

Resource::Resource(Game *owner_, const Name &name_): owner(owner_), name(name_) {}
Resource::Resource(Game *owner_, const char *name_): owner(owner_), name(name_) {}

Resource & Resource::add(const Name &name_, const Conversion &conversion) {
	conversions.emplace(name_, conversion);
	return *this;
}

bool Resource::hasType(const Type &type) const {
	return types.count(type) != 0;
}

bool Resource::hasType(Game &game, const Name &name, const Type &type) {
	return game.resources.at(name).hasType(type);
}
