#include <cmath>

#include "CentrifugeRecipe.h"
#include "Util.h"

CentrifugeOutput::CentrifugeOutput(const std::string &name_, double amount_, double chance_):
	name(name_), amount(amount_), chance(chance_) {}

CentrifugeRecipe::CentrifugeRecipe(const std::list<CentrifugeOutput> &outputs_):
	outputs(outputs_), probabilitySum(getSum(outputs_)) {}

std::pair<std::string, double> CentrifugeRecipe::choose() const {
	if (outputs.empty())
		throw std::runtime_error("Output list is empty.");
	const double chosen = randomRangeDouble(0., probabilitySum);
	double so_far = 0.;
	for (const CentrifugeOutput &output: outputs) {
		so_far += output.chance;
		if (chosen <= so_far)
			return {output.name, output.amount};
	}

	throw std::runtime_error("Couldn't choose a centrifuge output.");
}

double CentrifugeRecipe::getSum(const decltype(outputs) &list) const {
	double out = 0.;
	for (const CentrifugeOutput &output: list)
		out += output.chance;
	return out;
}
