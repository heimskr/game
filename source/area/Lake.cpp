#include "area/Lake.h"
#include "Region.h"
#include "Game.h"

LakeArea::LakeArea(Region *region, size_t size): Area(region, size) {
	resources.emplace("Water", static_cast<double>(size) * WATER_MAX);
}

std::string LakeArea::description() const {
	return "An expanse of fresh water.";
}

void LakeArea::tick(double delta) {
	if ((resources["Water"] += static_cast<double>(size) * 0.1 * delta) > static_cast<double>(size) * WATER_MAX)
		resources["Water"] = static_cast<double>(size) * WATER_MAX;
}
