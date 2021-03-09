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
	double &wood = resources["wood"];
	if ((wood += wood * .02 * delta) > static_cast<double>(size))
		wood = static_cast<double>(size);
}