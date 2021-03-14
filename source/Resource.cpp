#include "Game.h"
#include "Resource.h"

Resource::Resource(Game *owner_, const std::string &name_): owner(owner_), name(name_) {}
Resource::Resource(Game *owner_, const char *name_): owner(owner_), name(name_) {}

Resource & Resource::add(Processor::Type type, const Conversion &conversion) {
	switch (type) {
		// case Processor::Type::Furnace:    types.insert("smeltable");     break;
		case Processor::Type::Fermenter:  types.insert("fermentable");   break;
		// case Processor::Type::Centrifuge: types.insert("centrifugable"); break;
		default: break;
	}
	conversions.emplace(type, conversion);
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

Resource & Resource::setBasePrice(double base_price) {
	basePrice = base_price;
	return *this;
}
