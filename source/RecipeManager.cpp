#include "Game.h"
#include "RecipeManager.h"

RecipeManager::RecipeManager(Game &game_): game(&game_) {
	addAll();
}

void RecipeManager::addAll() {
	using ROV = std::vector<RandomOutput>;
	centrifuge.emplace("Stone", ROV {
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
	});
	centrifuge.emplace("Apple", ROV {{"", 0., 75.}, {"Malic Acid", 1., 25.}});
	centrifuge.emplace("Wood",  ROV {{"Cellulose", 1., 98.}, {"Lignin", 1., 2.}});
	centrifuge.emplace("Water", ROV {{"", 0., 95.}, {"Yeast", 1., 5.}});
	centrifuge.emplace("Fish",  ROV {{"", 0., 80.}, {"Blood", 5., 20.}});

	crusher.emplace("Stone", ROV {{"Sand", 2., 1.}});

	using RM = RefineryMode;
	using RG = RefineryRecipe::Group;
	refinery.reserve(1);
	refinery.emplace_back(RM::Polymerize, RG {{"Crude Oil", 1.}}, RG {{"Plastic", 3.}});

	crafting.emplace_back(Resource::Map {{"Plastic", 100.}, {"Silicon", 50.}}, "Microchip", 1.);

	using MOV = std::vector<MultiOutput>;
	electrolyzer.emplace("Water",      MOV {{"Hydrogen", 2.}, {"Oxygen", 1.}});
	electrolyzer.emplace("Coal",       MOV {{"Carbon", 10.}});
	electrolyzer.emplace("Malic Acid", MOV {{"Carbon", 4.}, {"Hydrogen", 6.}, {"Oxygen", 5.}});
	electrolyzer.emplace("Cellulose",  MOV {{"Carbon", 6.}, {"Hydrogen", 10.}, {"Oxygen", 5.}});
	electrolyzer.emplace("Lignin",     MOV {{"Carbon", 81.}, {"Hydrogen", 92.}, {"Oxygen", 28.}});
	electrolyzer.emplace("Blood",      MOV {{"Iron", 1.}});

	electrolyzer.emplace("Ammolite",      MOV {{"Calcium", 1.}, {"Carbon", 1.}, {"Oxygen", 3.}});
	electrolyzer.emplace("Azurite",       MOV {{"Copper", 3.}, {"Carbon", 2.}, {"Oxygen", 8.}, {"Hydrogen", 2.}});
	electrolyzer.emplace("Chrysoberyl",   MOV {{"Beryllium", 1.}, {"Aluminum", 2.}, {"Oxygen", 4.}});
	electrolyzer.emplace("Fluorite",      MOV {{"Calcium", 1.}, {"Fluorine", 2.}});
	electrolyzer.emplace("Lepidolite",    MOV {{"Potassium", 1.}, {"Lithium", 3.}, {"Aluminum", 7.}, {"Silicon", 4.}, {"Rubidium", 4.}, {"Oxygen", 12.}, {"Fluorine", 2.}, {"Hydrogen", 2.}});
	electrolyzer.emplace("Malachite",     MOV {{"Copper", 2.}, {"Carbon", 1.}, {"Oxygen", 5.}, {"Hydrogen", 2.}});
	electrolyzer.emplace("Opal",          MOV {{"Silicon", 1.}, {"Oxygen", 3.}, {"Hydrogen", 2.}});
	electrolyzer.emplace("Peridot",       MOV {{"Magnesium", 2.}, {"Iron", 2.}, {"Silicon", 1.}, {"Oxygen", 2.}});
	electrolyzer.emplace("Pyrope",        MOV {{"Magnesium", 3.}, {"Aluminum", 2.}, {"Silicon", 3.}, {"Oxygen", 12.}});
	electrolyzer.emplace("Rhodochrosite", MOV {{"Manganese", 1.}, {"Carbon", 1.}, {"Oxygen", 3.}});
}
