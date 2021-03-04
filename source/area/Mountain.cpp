#include "area/Mountain.h"
#include "Region.h"
#include "Game.h"

MountainArea::MountainArea(Region *region, size_t size): Area(region, size) {
	resources.emplace("ironOre", static_cast<double>(size));
	resources.emplace("coal", static_cast<double>(size));
}

std::string MountainArea::description() const {
	return "Resource-rich mountainous terrain.";
}

void MountainArea::tick() {}
