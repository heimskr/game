#include "Game.h"
#include "RecipeManager.h"

RecipeManager::RecipeManager(Game &game_): game(&game_) {
	addAll();
}

void RecipeManager::addAll() {
	centrifuge.emplace("Stone", std::list<CentrifugeOutput> {{"", 0.0, 0.9}, {"Silicon", 1.0, 0.1}});
}
