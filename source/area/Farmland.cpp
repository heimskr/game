#include "area/Farmland.h"
#include "Region.h"
#include "Game.h"

FarmlandArea::FarmlandArea(Region *region, size_t size, const std::string &resource_name):
		Area(region, size), resourceName(resource_name) {
	resources.emplace(resourceName, static_cast<double>(size) * FOOD_MAX / 4);
}

FarmlandArea::FarmlandArea(Region *region, size_t size):
	FarmlandArea(region, size, region->game->randomResource("farmable")) {}

std::string FarmlandArea::description() const {
	return "An area where food is grown.";
}

void FarmlandArea::tick(double delta) {
	if ((resources[resourceName] += static_cast<double>(size) * 0.1 * delta) > static_cast<double>(size) * FOOD_MAX)
		resources[resourceName] = static_cast<double>(size) * FOOD_MAX;
}

std::string FarmlandArea::toString() const {
	return Area::toString() + ":" + resourceName;
}
