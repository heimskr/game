#include "area/Housing.h"
#include "Region.h"
#include "Game.h"

HousingArea::HousingArea(Region *region, size_t size): Area(region, size) {
	resources.emplace("Human", static_cast<double>(size / 4));
	spawnGoods();
}

std::string HousingArea::description() const {
	return "A populated area.";
}

void HousingArea::tick(double delta) {
	double &humans = resources["Human"];
	if (2. <= humans)
		if ((humans += humans * 0.002 * delta) > static_cast<double>(size))
			humans = static_cast<double>(size);
	if (1. <= humans) {
		Resource::Map non_owned = parent->allNonOwnedResources();
		std::list<std::pair<const std::string *, double>> subtractions;
		for (const auto &[name, amount]: non_owned) {
			const Resource &resource = parent->game->resources.at(name);
			if (resource.hasType("food") || resource.hasType("drink"))
				subtractions.emplace_back(&name, std::min(size / 500., amount));
		}
		for (const auto &subtraction: subtractions)
			parent->subtractResourceFromNonOwned(*subtraction.first, subtraction.second);
	}
}

void HousingArea::spawnGoods() {
	for (const auto &[name, resource]: parent->game->resources)
		if (resource.hasType("rare spawnable")) {
			if (chance(0.1))
				resources[name] += randomRange(1, 7) % 5;
		} else if (resource.hasType("uncommon spawnable")) {
			if (chance(0.25))
				resources[name] += randomRange(2, 10);
		} else if (resource.hasType("common spawnable")) {
			if (chance(0.5))
				resources[name] += randomRange(5, 20);
		}
}
