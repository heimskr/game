#include "area/Lake.h"
#include "Region.h"
#include "Game.h"

LakeArea::LakeArea(Region *region, size_t size): Area(region, size) {
	resources.emplace("Water", static_cast<double>(size) * WATER_MAX);
	resources.emplace("Fish", static_cast<double>(size) * FISH_DENSITY / 2.);
}

std::string LakeArea::description() const {
	return "An expanse of fresh water.";
}

void LakeArea::tick(double delta) {
	double &water = resources["Water"];
	if ((water += static_cast<double>(size) * 0.1 * delta) > static_cast<double>(size) * WATER_MAX)
		water = static_cast<double>(size) * WATER_MAX;
	double &fish = resources["Fish"];
	if (2.0 <= fish && (fish += fish * 0.005 * delta) > static_cast<double>(size) * FISH_DENSITY)
		fish = static_cast<double>(size) * FISH_DENSITY;
}
