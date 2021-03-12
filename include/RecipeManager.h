#pragma once

#include <vector>

#include "CentrifugeRecipe.h"

class Game;

struct RecipeManager {
	Game *game;
	std::vector<CentrifugeRecipe> centrifuge;

	RecipeManager() = delete;
	RecipeManager(Game &);

	void addAll();
};