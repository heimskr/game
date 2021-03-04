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
	double &humans = resources["human"];
	if (2.0 <= humans)
		if ((humans *= 1.002) > static_cast<double>(size))
			humans = static_cast<double>(size);
}
