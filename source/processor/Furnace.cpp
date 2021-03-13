#include <list>

#include "Game.h"
#include "Resource.h"
#include "processor/Furnace.h"

Furnace::Furnace(Game &game_, double fuel_, const std::map<std::string, double> &input_, const std::map<std::string, double> &output_, bool auto_extract):
	Processor(game_, input_, output_, auto_extract), fuel(fuel_) {}

Furnace::Furnace(Game &game_): Processor(game_) {}

std::string Furnace::toString() const {
	return Processor::toString() + ":" + std::to_string(fuel);
}

double Furnace::tick(double delta) {
	for (auto &[name, amount]: input) {
		const Resource &resource = game->resources.at(name);
		if (resource.hasType("fuel")) {
			const double to_remove = std::min(delta, amount);
			amount -= to_remove;
			fuel += to_remove;
		}
	}

	double out = 0.;

	if (fuel < Resource::MIN_AMOUNT) {
		fuel = 0.;
	} else {
		for (auto &[name, amount]: input) {
			const Resource &resource = game->resources.at(name);
			if (resource.conversions.count(getType()) == 0)
				continue;
			const auto &conversion = resource.conversions.at(getType());
			const double to_convert = std::min(fuel, std::min(amount, conversion.rate * delta));
			out += to_convert;
			fuel -= to_convert;
			amount -= to_convert;
			output[conversion.outName] += to_convert * conversion.amount;
			if (fuel < Resource::MIN_AMOUNT)
				break;
		}
		moveOutput();
	}

	shrink(input);
	return out;
}
