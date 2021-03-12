#include <list>

#include "Game.h"
#include "Resource.h"
#include "processor/Centrifuge.h"

double Centrifuge::tick(double delta) {
	double out = 0.;

	const auto &recipes = owner->recipes.centrifuge;

	for (auto &[name, amount]: input)
		if (recipes.count(name) != 0) {
			const std::pair<std::string, double> choice = recipes.at(name).choose();
			// TODO: possibly allow centrifuge recipes to consume more than 1 of the resource per second
			const double to_convert = std::min(amount, delta);
			output[choice.first] += to_convert * choice.second;
			amount -= to_convert;
			out += to_convert;
		}

	shrink(input);
	return out;
}
