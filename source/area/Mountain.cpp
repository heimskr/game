#include "area/Mountain.h"
#include "Region.h"
#include "Game.h"

MountainArea::MountainArea(Region *region, size_t size): Area(region, size) {
	resources.emplace("ironOre", static_cast<double>(size) * IRON_MAX);
	resources.emplace("coal", static_cast<double>(size) * COAL_MAX);
}

std::string MountainArea::description() const {
	return "Resource-rich mountainous terrain.";
}

void MountainArea::tick() {
	if ((resources["coal"] += static_cast<double>(size) * 0.01) > static_cast<double>(size) * COAL_MAX)
		 resources["coal"]  = static_cast<double>(size) * COAL_MAX;
	if ((resources["ironOre"] += static_cast<double>(size) * 0.01) > static_cast<double>(size) * IRON_MAX)
		 resources["ironOre"]  = static_cast<double>(size) * IRON_MAX;
}
