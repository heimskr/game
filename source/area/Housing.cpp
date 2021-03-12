#include "area/Housing.h"
#include "Region.h"
#include "Game.h"

HousingArea::HousingArea(Region *region, size_t size): Area(region, size) {
	resources.emplace("Human", static_cast<double>(size / 4));
}

std::string HousingArea::description() const {
	return "A populated area.";
}

void HousingArea::tick(double delta) {
	double &humans = resources["Human"];
	if (2.0 <= humans)
		if ((humans += humans * .002 * delta) > static_cast<double>(size))
			humans = static_cast<double>(size);
}
