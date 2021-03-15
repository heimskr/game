#include "Game.h"
#include "RecipeManager.h"

RecipeManager::RecipeManager(Game &game_): game(&game_) {
	addAll();
}

void RecipeManager::addAll() {
	using ROV = std::vector<RandomOutput>;
	centrifuge.emplace("Stone",      std::move(ROV {{"", 0., 8.}, {"Silicon", 1., 1.}, {"Oxygen", 2., 1.}}));
	centrifuge.emplace("Apple",      std::move(ROV {{"", 0., 75.}, {"Malic Acid", 1., 25.}}));
	centrifuge.emplace("Malic Acid", std::move(ROV {{"Carbon", 4., 4.}, {"Hydrogen", 6., 6.}, {"Oxygen", 5., 5.}}));
	centrifuge.emplace("Coal",       std::move(ROV {{"Carbon", 10., 1.}}));
	centrifuge.emplace("Wood",       std::move(ROV {{"Cellulose", 1., 98.}, {"Lignin", 1., 2.}}));
	centrifuge.emplace("Cellulose",  std::move(ROV {{"Carbon", 6., 6.}, {"Hydrogen", 10., 10.}, {"Oxygen", 5., 5.}}));
	centrifuge.emplace("Lignin",     std::move(ROV {{"Carbon", 81., 81.}, {"Hydrogen", 92., 92.}, {"Oxygen", 28., 28.}}));
	centrifuge.emplace("Water",      std::move(ROV {{"", 0., 95.}, {"Yeast", 1., 5.}}));

	crusher.emplace("Stone", std::move(ROV {{"Sand", 2., 1.}}));

	using RM = RefineryMode;
	using RG = RefineryRecipe::Group;
	refinery.reserve(1);
	refinery.emplace_back(RM::Polymerize, std::move(RG {{"Crude Oil", 1.}}), std::move(RG {{"Plastic", 3.}}));

	crafting.emplace_back(std::move(Resource::Map {{"Plastic", 100.}, {"Silicon", 50.}}), "Microchip", 1.);
}
