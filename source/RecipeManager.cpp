#include "Game.h"
#include "RecipeManager.h"

RecipeManager::RecipeManager(Game &game_): game(&game_) {
	addAll();
}

void RecipeManager::addAll() {
	using ROV = std::vector<RandomOutput>;
	centrifuge.emplace("Stone",      std::move(ROV {{"", 0., 8.}, {"Silicon", 1., 1.}, {"Oxygen", 2., 1.}}));
	centrifuge.emplace("Apple",      std::move(ROV {{"", 0., 75.}, {"Malic Acid", 1., 25.}}));
	centrifuge.emplace("Wood",       std::move(ROV {{"Cellulose", 1., 98.}, {"Lignin", 1., 2.}}));
	centrifuge.emplace("Water",      std::move(ROV {{"", 0., 95.}, {"Yeast", 1., 5.}}));
	centrifuge.emplace("Fish",       std::move(ROV {{"", 0., 80.}, {"Blood", 5., 20.}}));

	crusher.emplace("Stone", std::move(ROV {{"Sand", 2., 1.}}));

	using RM = RefineryMode;
	using RG = RefineryRecipe::Group;
	refinery.reserve(1);
	refinery.emplace_back(RM::Polymerize, std::move(RG {{"Crude Oil", 1.}}), std::move(RG {{"Plastic", 3.}}));

	crafting.emplace_back(std::move(Resource::Map {{"Plastic", 100.}, {"Silicon", 50.}}), "Microchip", 1.);

	using MOV = std::vector<MultiOutput>;
	electrolyzer.emplace("Water",      std::move(MOV {{"Hydrogen", 2.}, {"Oxygen", 1.}}));
	electrolyzer.emplace("Coal",       std::move(MOV {{"Carbon", 10.}}));
	electrolyzer.emplace("Malic Acid", std::move(MOV {{"Carbon", 4.}, {"Hydrogen", 6.}, {"Oxygen", 5.}}));
	electrolyzer.emplace("Cellulose",  std::move(MOV {{"Carbon", 6.}, {"Hydrogen", 10.}, {"Oxygen", 5.}}));
	electrolyzer.emplace("Lignin",     std::move(MOV {{"Carbon", 81.}, {"Hydrogen", 92.}, {"Oxygen", 28.}}));
	electrolyzer.emplace("Blood",      std::move(MOV {{"Iron", 1.}}));
}
