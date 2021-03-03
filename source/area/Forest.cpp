#include "area/Forest.h"
#include "Region.h"
#include "Game.h"

ForestArea::ForestArea(Region *region, size_t size): Area(region, size) {
	resources.emplace("wood", static_cast<double>(size));
}

void ForestArea::tick() {
	if ((resources["wood"] *= 1.05) > static_cast<double>(size))
		resources["wood"] = static_cast<double>(size);
}
