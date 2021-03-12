#include "Game.h"
#include "RecipeManager.h"

RecipeManager::RecipeManager(Game &game_): game(&game_) {
	centrifuge.reserve(16);
}

void RecipeManager::addAll() {
	centrifuge.push_back({"stone", {{"", 0.0, 0.9}, {"silicon", 1.0, 0.1}}});
}
