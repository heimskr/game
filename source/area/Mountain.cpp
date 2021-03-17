#include "area/Mountain.h"
#include "Region.h"
#include "Game.h"

MountainArea::MountainArea(Region *region, size_t size): Area(region, size) {
	resources.emplace("Iron Ore", static_cast<double>(size) * IRON_MAX);
	resources.emplace("Coal", static_cast<double>(size) * COAL_MAX);
	resources.emplace("Stone", static_cast<double>(size) * STONE_MAX);
}

std::string MountainArea::description() const {
	return "Resource-rich mountainous terrain.";
}

void MountainArea::tick(double delta) {
	if ((resources["Coal"] += static_cast<double>(size) * 0.0005 * delta) > static_cast<double>(size) * COAL_MAX)
		 resources["Coal"]  = static_cast<double>(size) * COAL_MAX;
	if ((resources["Iron Ore"] += static_cast<double>(size) * 0.0005 * delta) > static_cast<double>(size) * IRON_MAX)
		 resources["Iron Ore"]  = static_cast<double>(size) * IRON_MAX;
	if ((resources["Stone"] += static_cast<double>(size) * 0.0005 * delta) > static_cast<double>(size) * STONE_MAX)
		 resources["Stone"]  = static_cast<double>(size) * STONE_MAX;
}
