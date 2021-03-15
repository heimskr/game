#include "area/OilField.h"
#include "Region.h"
#include "Game.h"

OilFieldArea::OilFieldArea(Region *region, size_t size): Area(region, size) {
	resources.emplace("Crude Oil", static_cast<double>(size) * OIL_MAX);
}

std::string OilFieldArea::description() const {
	return "Oil-rich terrain.";
}

void OilFieldArea::tick(double delta) {
	if ((resources["Crude Oil"] += static_cast<double>(size) * 0.001 * delta) > static_cast<double>(size) * OIL_MAX)
		 resources["Crude Oil"]  = static_cast<double>(size) * OIL_MAX;
}
