#include "recipe/CraftingRecipe.h"

CraftingRecipe::CraftingRecipe(const std::map<std::string, double> &inputs_, const char *output_, double amount_):
	inputs(inputs_), output(output_), amount(amount_) {}

CraftingRecipe::CraftingRecipe(std::map<std::string, double> &&inputs_, const char *output_, double amount_):
	inputs(std::move(inputs_)), output(output_), amount(amount_) {}
