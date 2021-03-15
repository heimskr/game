#include <cmath>

#include "recipe/RandomRecipe.h"
#include "Util.h"

RandomOutput::RandomOutput(const std::string &name_, double amount_, double chance_):
	name(name_), amount(amount_), chance(chance_) {}

RandomRecipe::RandomRecipe(const std::vector<RandomOutput> &outputs_):
	outputs(outputs_), probabilitySum(getSum(outputs_)) {}

RandomRecipe::RandomRecipe(std::vector<RandomOutput> &&outputs_):
	outputs(std::move(outputs_)), probabilitySum(getSum(outputs)) {}

std::pair<std::string, double> RandomRecipe::choose() const {
	if (outputs.empty())
		throw std::runtime_error("Output list is empty.");
	const double chosen = randomRangeDouble(0., probabilitySum);
	double so_far = 0.;
	for (const RandomOutput &output: outputs) {
		so_far += output.chance;
		if (chosen <= so_far)
			return {output.name, output.amount};
	}

	throw std::runtime_error("Couldn't choose a recipe output.");
}

double RandomRecipe::getSum(const decltype(outputs) &list) const {
	double out = 0.;
	for (const RandomOutput &output: list)
		out += output.chance;
	return out;
}
