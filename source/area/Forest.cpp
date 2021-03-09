#include "area/Forest.h"
#include "Region.h"
#include "Game.h"

ForestArea::ForestArea(Region *region, size_t size): Area(region, size) {
	resources.emplace("wood", static_cast<double>(size));
}

std::string ForestArea::description() const {
	return "A quiet wooded area.";
}

void ForestArea::tick(double delta) {
	if ((resources["wood"] += resources["wood"] * .02 * delta) > static_cast<double>(size))
		resources["wood"] = static_cast<double>(size);
}