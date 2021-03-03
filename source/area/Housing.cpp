#include "area/Housing.h"
#include "Region.h"
#include "Game.h"

HousingArea::HousingArea(Region *region, size_t size): Area(region, size) {
	resources.emplace("human", static_cast<double>(size / 4));
}

std::string HousingArea::description() const {
	return "A populated area.";
}

void HousingArea::tick() {
	if ((resources["human"] *= 1.01) > static_cast<double>(size))
		resources["human"] = static_cast<double>(size);
}
