#include "Game.h"
#include "RecipeManager.h"

RecipeManager::RecipeManager(Game &game_): game(&game_) {
	addAll();
}

void RecipeManager::addAll() {
	centrifuge.emplace("Stone", std::list<CentrifugeOutput> {{"", 0.0, 0.9}, {"Silicon", 1.0, 0.1}});
	centrifuge.emplace("Apple", std::list<CentrifugeOutput> {{"", 0.0, 0.75}, {"Malic Acid", 1.0, 0.25}});
	centrifuge.emplace("Malic Acid", std::list<CentrifugeOutput> {{"Carbon", 4., 4.}, {"Hydrogen", 6., 6.}, {"Oxygen", 5., 5.}});
}
