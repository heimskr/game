#pragma once

#include <vector>

#include "recipe/MultiOutput.h"

struct ElectrolyzerRecipe {
	std::vector<MultiOutput> outputs;

	ElectrolyzerRecipe(const std::vector<MultiOutput> &outputs_): outputs(outputs_) {}
	ElectrolyzerRecipe(std::vector<MultiOutput> &&outputs_): outputs(std::move(outputs_)) {}
};
