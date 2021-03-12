#pragma once

#include <map>

#include "CentrifugeRecipe.h"

class Game;

struct RecipeManager {
	Game *game;
	std::map<std::string, CentrifugeRecipe> centrifuge;

	RecipeManager() = delete;
	RecipeManager(Game &);

	void addAll();
};