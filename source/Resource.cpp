#include "Game.h"
#include "Resource.h"

Resource::Resource(const Name &name_): name(name_) {}
Resource::Resource(const char *name_): name(name_) {}

Resource & Resource::add(const Name &name_, const Conversion &conversion) {
	conversions.emplace(name_, conversion);
	return *this;
}

bool Resource::hasType(const Type &type) const {
	return types.count(type) != 0;
}

bool Resource::hasType(const Name &name, const Type &type) {
	return Globals::resources.at(name).hasType(type);
}
