#pragma once

#include <map>

#include "recipe/CentrifugeRecipe.h"
#include "recipe/CrusherRecipe.h"
#include "recipe/RefineryRecipe.h"
#include "recipe/CraftingRecipe.h"

class Game;

struct RecipeManager {
	Game *game;
	std::map<std::string, CentrifugeRecipe> centrifuge;
	std::map<std::string, CrusherRecipe> crusher;
	std::vector<RefineryRecipe> refinery;
	std::vector<CraftingRecipe> crafting;

	RecipeManager() = delete;
	RecipeManager(Game &);

	void addAll();
};