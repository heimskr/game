#include "Game.h"
#include "RecipeManager.h"

RecipeManager::RecipeManager(Game &game_): game(&game_) {
	addAll();
}

void RecipeManager::addAll() {
	using ROV = std::vector<RandomOutput>;
	centrifuge.emplace("Stone",      std::move(ROV {
		{"", 0., 100.},
		{"Ammolite",    1., 1.},
		{"Azurite",     1., 1.},
		{"Chrysoberyl", 1., 0.01},
		{"Fluorite",    1., 1.},
		{"Lepidolite",  1., 0.001},
		{"Malachite",   1., 1.},
		{"Opal",        1., 1.},
		{"Peridot",     1., 1.},
		{"Pyrope",      1., 1.},
	}));
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

	electrolyzer.emplace("Ammolite",      std::move(MOV {{"Calcium", 1.}, {"Carbon", 1.}, {"Oxygen", 3.}}));
	electrolyzer.emplace("Azurite",       std::move(MOV {{"Copper", 3.}, {"Carbon", 2.}, {"Oxygen", 8.}, {"Hydrogen", 2.}}));
	electrolyzer.emplace("Chrysoberyl",   std::move(MOV {{"Beryllium", 1.}, {"Aluminum", 2.}, {"Oxygen", 4.}}));
	electrolyzer.emplace("Fluorite",      std::move(MOV {{"Calcium", 1.}, {"Fluorine", 2.}}));
	electrolyzer.emplace("Lepidolite",    std::move(MOV {{"Potassium", 1.}, {"Lithium", 3.}, {"Aluminum", 7.}, {"Silicon", 4.}, {"Rubidium", 4.}, {"Oxygen", 12.}, {"Fluorine", 2.}, {"Hydrogen", 2.}}));
	electrolyzer.emplace("Malachite",     std::move(MOV {{"Copper", 2.}, {"Carbon", 1.}, {"Oxygen", 5.}, {"Hydrogen", 2.}}));
	electrolyzer.emplace("Opal",          std::move(MOV {{"Silicon", 1.}, {"Oxygen", 3.}, {"Hydrogen", 2.}}));
	electrolyzer.emplace("Peridot",       std::move(MOV {{"Magnesium", 2.}, {"Iron", 2.}, {"Silicon", 1.}, {"Oxygen", 2.}}));
	electrolyzer.emplace("Pyrope",        std::move(MOV {{"Magnesium", 3.}, {"Aluminum", 2.}, {"Silicon", 3.}, {"Oxygen", 12.}}));
	electrolyzer.emplace("Rhodochrosite", std::move(MOV {{"Manganese", 1.}, {"Carbon", 1.}, {"Oxygen", 3.}}));
}
