#include "area/Forest.h"
#include "Region.h"
#include "Game.h"

ForestArea::ForestArea(Region *region, size_t size): Area(region, size) {
	resources.emplace("wood", static_cast<double>(size));
}

std::string ForestArea::description() const {
	return "A quiet wooded area.";
}

void ForestArea::tick() {
	if ((resources["wood"] *= 1.02) > static_cast<double>(size))
		resources["wood"] = static_cast<double>(size);
}
