#pragma once

#include <map>

#include "recipe/CentrifugeRecipe.h"
#include "recipe/CrusherRecipe.h"
#include "recipe/RefineryRecipe.h"

class Game;

struct RecipeManager {
	Game *game;
	std::map<std::string, CentrifugeRecipe> centrifuge;
	std::map<std::string, CrusherRecipe> crusher;
	std::map<std::string, RefineryRecipe> refinery;

	RecipeManager() = delete;
	RecipeManager(Game &);

	template <typename... Args>
	void addCentrifuge(const char *input, Args &&...args) {
		centrifuge.emplace(input, std::move(std::vector<RandomOutput> {std::forward(args)...}));
	}

	void addAll();
};