#include "Game.h"
#include "RecipeManager.h"

RecipeManager::RecipeManager(Game &game_): game(&game_) {
	addAll();
}

void RecipeManager::addAll() {
	// centrifuge.emplace("Stone",      std::move(std::vector<RandomOutput> {{"", 0., 8.}, {"Silicon", 1., 1.}, {"Oxygen", 2., 1.}}));
	// centrifuge.emplace("Apple",      std::move(std::vector<RandomOutput> {{"", 0., 75.}, {"Malic Acid", 1., 25.}}));
	// centrifuge.emplace("Malic Acid", std::move(std::vector<RandomOutput> {{"Carbon", 4., 4.}, {"Hydrogen", 6., 6.}, {"Oxygen", 5., 5.}}));
	// centrifuge.emplace("Coal",       std::move(std::vector<RandomOutput> {{"Carbon", 10., 1.}}));
	// centrifuge.emplace("Wood",       std::move(std::vector<RandomOutput> {{"Cellulose", 1., 98.}, {"Lignin", 1., 2.}}));
	// centrifuge.emplace("Cellulose",  std::move(std::vector<RandomOutput> {{"Carbon", 6., 6.}, {"Hydrogen", 10., 10.}, {"Oxygen", 5., 5.}}));
	// centrifuge.emplace("Lignin",     std::move(std::vector<RandomOutput> {{"Carbon", 81., 81.}, {"Hydrogen", 92., 92.}, {"Oxygen", 28., 28.}}));
	// centrifuge.emplace("Water",      std::move(std::vector<RandomOutput> {{"", 0., 95.}, {"Yeast", 1., 5.}}));
	addCentrifuge("Stone",      {"", 0., 8.}, {"Silicon", 1., 1.}, {"Oxygen", 2., 1.});
	addCentrifuge("Apple",      {"", 0., 75.}, {"Malic Acid", 1., 25.});
	addCentrifuge("Malic Acid", {"Carbon", 4., 4.}, {"Hydrogen", 6., 6.}, {"Oxygen", 5., 5.});
	addCentrifuge("Coal",       {"Carbon", 10., 1.});
	addCentrifuge("Wood",       {"Cellulose", 1., 98.}, {"Lignin", 1., 2.});
	addCentrifuge("Cellulose",  {"Carbon", 6., 6.}, {"Hydrogen", 10., 10.}, {"Oxygen", 5., 5.});
	addCentrifuge("Lignin",     {"Carbon", 81., 81.}, {"Hydrogen", 92., 92.}, {"Oxygen", 28., 28.});
	addCentrifuge("Water",      {"", 0., 95.}, {"Yeast", 1., 5.});

	crusher.emplace("Stone", std::list<RandomOutput> {{"Sand", 2., 1.}});
}
