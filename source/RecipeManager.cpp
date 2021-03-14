#include "Game.h"
#include "RecipeManager.h"

RecipeManager::RecipeManager(Game &game_): game(&game_) {
	addAll();
}

void RecipeManager::addAll() {
	centrifuge.emplace("Stone",      std::list<RandomOutput> {{"", 0., 8.}, {"Silicon", 1., 1.}, {"Oxygen", 2., 1.}});
	centrifuge.emplace("Apple",      std::list<RandomOutput> {{"", 0., 75.}, {"Malic Acid", 1., 25.}});
	centrifuge.emplace("Malic Acid", std::list<RandomOutput> {{"Carbon", 4., 4.}, {"Hydrogen", 6., 6.}, {"Oxygen", 5., 5.}});
	centrifuge.emplace("Coal",       std::list<RandomOutput> {{"Carbon", 10., 1.}});
	centrifuge.emplace("Wood",       std::list<RandomOutput> {{"Cellulose", 1., 98.}, {"Lignin", 1., 2.}});
	centrifuge.emplace("Cellulose",  std::list<RandomOutput> {{"Carbon", 6., 6.}, {"Hydrogen", 10., 10.}, {"Oxygen", 5., 5.}});
	centrifuge.emplace("Lignin",     std::list<RandomOutput> {{"Carbon", 81., 81.}, {"Hydrogen", 92., 92.}, {"Oxygen", 28., 28.}});
	centrifuge.emplace("Water",      std::list<RandomOutput> {{"", 0., 95.}, {"Yeast", 1., 5.}});

	crusher.emplace("Stone", std::list<RandomOutput> {{"Sand", 2., 1.}});
}
