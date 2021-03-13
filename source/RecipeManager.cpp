#include "Game.h"
#include "RecipeManager.h"

RecipeManager::RecipeManager(Game &game_): game(&game_) {
	addAll();
}

void RecipeManager::addAll() {
	centrifuge.emplace("Stone",      std::list<CentrifugeOutput> {{"", 0., 0.8}, {"Silicon", 1., 0.1}, {"Oxygen", 2., 0.1}});
	centrifuge.emplace("Apple",      std::list<CentrifugeOutput> {{"", 0., 0.75}, {"Malic Acid", 1., 0.25}});
	centrifuge.emplace("Malic Acid", std::list<CentrifugeOutput> {{"Carbon", 4., 4.}, {"Hydrogen", 6., 6.}, {"Oxygen", 5., 5.}});
	centrifuge.emplace("Coal",       std::list<CentrifugeOutput> {{"Carbon", 10., 1.}});
	centrifuge.emplace("Wood",       std::list<CentrifugeOutput> {{"Cellulose", 1., 0.98}, {"Lignin", 1., 0.02}});
	centrifuge.emplace("Cellulose",  std::list<CentrifugeOutput> {{"Carbon", 6., 6.}, {"Hydrogen", 10., 10.}, {"Oxygen", 5., 5.}});
	centrifuge.emplace("Lignin",     std::list<CentrifugeOutput> {{"Carbon", 81., 81.}, {"Hydrogen", 92., 92.}, {"Oxygen", 28., 28.}});
	centrifuge.emplace("Water",      std::list<CentrifugeOutput> {{"", 0., 0.95}, {"Yeast", 1., 0.05}});
}
