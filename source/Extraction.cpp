#include "Area.h"
#include "Extraction.h"
#include "Game.h"
#include "Region.h"
#include "Util.h"

std::string Extraction::toString() const {
	return area->parent->name + ":" + area->name + ":" + resourceName + ":" + std::to_string(amount) + ":" + std::to_string(rate);
}

Extraction Extraction::fromString(const Game &game, const std::string &str) {
	const std::vector<std::string> pieces = split(str, ":", false);
	const Region *region = nullptr;
	for (const auto &pair: game.regions)
		if (pair.second.name == pieces[0])
			region = &pair.second;
	if (!region)
		throw std::runtime_error("Couldn't find Region for extraction");
	if (region->areas.count(pieces[1]) == 0)
		throw std::runtime_error("Couldn't find Area for extraction");
	const Area *area = region->areas.at(pieces[1]).get();
	return {const_cast<Area *>(area), pieces[2], parseDouble(pieces[3]), parseDouble(pieces[4])};
}
