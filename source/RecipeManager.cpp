#include "Game.h"
#include "RecipeManager.h"

RecipeManager::RecipeManager(Game &game_): game(&game_) {
	addAll();
}

void RecipeManager::addAll() {
	centrifuge.emplace("stone", std::list<CentrifugeOutput> {{"", 0.0, 0.9}, {"silicon", 1.0, 0.1}});
}
