#include "area/Lake.h"
#include "Region.h"
#include "Game.h"

LakeArea::LakeArea(Region *region, size_t size): Area(region, size) {
	resources.emplace("water", static_cast<double>(size) * WATER_MAX);
}

std::string LakeArea::description() const {
	return "An expanse of fresh water.";
}

void LakeArea::tick() {
	if ((resources["water"] *= 1.03) > static_cast<double>(size) * WATER_MAX)
		resources["water"] = static_cast<double>(size) * WATER_MAX;
}
