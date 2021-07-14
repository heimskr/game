#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <switch.h>

#include "FS.h"
#include "Game.h"
#include "Keyboard.h"
#include "NameGen.h"
#include "Util.h"
#include "Processor.h"
#include "area/Areas.h"
#include "processor/Processors.h"

Game::Game() {
	addAll();
}

void Game::add(const Resource &resource) {
	resources.emplace(resource.name, resource);
}

static constexpr double ELEMENT_BASE = 3.;

long double operator""_e(long double price) {
	return price / 0.424 * ELEMENT_BASE;
	// 0.424: Price of iron (USD/kg).
	// I still need to decide whether units in this game represent mass or volume.
}

void Game::addResources() {
	add(Resource(this, "Apple").setBasePrice(0.5).addTypes("farmable", "food", "centrifugable"));
	add(Resource(this, "Blood").setBasePrice(10.).addTypes("liquid"));
	add(Resource(this, "Cellulose").setBasePrice(1.0).addTypes("molecule", "chemical", "centrifugable"));
	add(Resource(this, "Charcoal").setBasePrice(1.5).addTypes("fuel"));
	add(Resource(this, "Coal").setBasePrice(2.).addTypes("fuel", "smeltable").setDefaultExtractionRate(0.5));
	add(Resource(this, "Crude Oil").setBasePrice(5.));
	add(Resource(this, "Fish").setBasePrice(5.).addTypes("living"));
	add(Resource(this, "Glass").setBasePrice(1.));
	add(Resource(this, "Honey").setBasePrice(0.75).addTypes("farmable", "food").add(Processor::Type::Fermenter, {0.1, "Mead"}));
	add(Resource(this, "Human").setBasePrice(9999.).addTypes("alive", "sapient"));
	add(Resource(this, "Iron Ore").setBasePrice(2.).addTypes("ore", "iron", "smeltable", "rocket smeltable").add(Processor::Type::Furnace, {0.5, "Iron"}).setDefaultExtractionRate(0.5));
	add(Resource(this, "Lignin").setBasePrice(10.0).addTypes("molecule", "chemical", "centrifugable"));
	add(Resource(this, "Malic Acid").setBasePrice(1.0).addTypes("molecule", "chemical", "centrifugable"));
	add(Resource(this, "Mead").setBasePrice(25.).addTypes("drink", "alcohol"));
	add(Resource(this, "Microchip").setBasePrice(300.).addTypes("rare spawnable"));
	add(Resource(this, "Plastic").setBasePrice(2.5));
	add(Resource(this, "Sand").setBasePrice(0.6).addTypes("smeltable", "rocket smeltable").add(Processor::Type::Furnace, {0.25, "Glass"}));
	add(Resource(this, "Stone").setBasePrice(0.1).setDefaultExtractionRate(0.5));
	add(Resource(this, "Water").setBasePrice(0.05).addTypes("liquid", "centrifugable"));
	add(Resource(this, "Wheat").setBasePrice(0.5).addTypes("farmable"));
	add(Resource(this, "Wood").setBasePrice(1.).addTypes("fuel", "natural").add(Processor::Type::Furnace, {1., "Charcoal"}));
	add(Resource(this, "Yeast").setBasePrice(1.).addTypes("alive", "microorganism", "fermentable"));

	add(Resource(this, "Aluminum").setBasePrice(1.79_e).addTypes("element", "chemical", "metal"));
	add(Resource(this, "Beryllium").setBasePrice(857._e).addTypes("element", "chemical", "metal"));
	add(Resource(this, "Calcium").setBasePrice(2.28_e).addTypes("element", "chemical"));
	add(Resource(this, "Carbon").setBasePrice(0.122_e).addTypes("element", "chemical"));
	add(Resource(this, "Copper").setBasePrice(6._e).addTypes("element", "chemical", "metal"));
	add(Resource(this, "Fluorine").setBasePrice(2._e).addTypes("element", "chemical"));
	add(Resource(this, "Hydrogen").setBasePrice(1.39_e).addTypes("element", "chemical"));
	add(Resource(this, "Iron").setBasePrice(ELEMENT_BASE).addTypes("element", "chemical", "metal", "iron"));
	add(Resource(this, "Lithium").setBasePrice(83.5_e).addTypes("element", "chemical"));
	add(Resource(this, "Rubidium").setBasePrice(15'500._e).addTypes("element", "chemical", "metal"));
	add(Resource(this, "Magnesium").setBasePrice(2.32_e).addTypes("element", "chemical", "metal"));
	add(Resource(this, "Manganese").setBasePrice(1.82_e).addTypes("element", "chemical", "metal"));
	add(Resource(this, "Oxygen").setBasePrice(0.154_e).addTypes("element", "chemical"));
	add(Resource(this, "Potassium").setBasePrice(12.85_e).addTypes("element", "chemical"));
	add(Resource(this, "Silicon").setBasePrice(1.7_e).addTypes("element", "chemical"));

	add(Resource(this, "Ammolite").setBasePrice(21.).addTypes("mineral").setDescription("CaCO₃"));
	add(Resource(this, "Azurite").setBasePrice(158.).addTypes("mineral").setDescription("Cu₃(CO₃)₂(OH)₂"));
	add(Resource(this, "Chrysoberyl").setBasePrice(6094.).addTypes("mineral").setDescription("BeAl₂O₄"));
	add(Resource(this, "Fluorite").setBasePrice(45.).addTypes("mineral").setDescription("CaF₂"));
	add(Resource(this, "Lepidolite").setBasePrice(440742.).addTypes("mineral").setDescription("K(Li,Al)₃(Al,Si,Rb)₄O₁₀(F,OH)₂"));
	add(Resource(this, "Malachite").setBasePrice(111.).addTypes("mineral").setDescription("Cu₂CO₃(OH)₂"));
	add(Resource(this, "Opal").setBasePrice(35.).addTypes("mineral").setDescription("SiO₂ + H₂O"));
	add(Resource(this, "Peridot").setBasePrice(56.).addTypes("mineral").setDescription("(Mg,Fe)₂SiO₄"));
	add(Resource(this, "Pyrope").setBasePrice(124.).addTypes("mineral").setDescription("Mg₃Al₂(SiO₄)₃"));
	add(Resource(this, "Rhodochrosite").setBasePrice(18.).addTypes("mineral").setDescription("MnCO₃"));
}

void Game::add(Processor::Type type, const Resource::Map &cost) {
	processorCosts.emplace(type, cost);
}

void Game::addProcessorCosts() {
	add(Processor::Type::Furnace,       {{"Stone", 100.}});
	add(Processor::Type::Centrifuge,    {{"Iron",  200.}});
	add(Processor::Type::Fermenter,     {{"Glass", 100.}});
	add(Processor::Type::Crusher,       {{"Iron",  300.}});
	add(Processor::Type::Refinery,      {{"Iron",  500.}, {"Microchip", 2.}});
	add(Processor::Type::RocketFurnace, {{"Iron",  500.}, {"Plastic", 500.}, {"Microchip", 10.}});
	add(Processor::Type::Electrolyzer,  {{"Iron",  500.}, {"Glass", 500.}});
}

void Game::addAll() {
	if (!ready) {
		addResources();
		addProcessorCosts();
		ready = true;
	}
}

std::string Game::randomResource(const Resource::Type &type) const {
	std::vector<std::string> choices;
	choices.reserve(resources.size());
	for (const auto &[name, resource]: resources) {
		if (resource.types.count(type) != 0)
			choices.push_back(name);
	}
	if (choices.empty())
		return "";
	return choices.at(randomRange(0ul, choices.size() - 1ul));
}

bool Game::updatePosition(Region &region, const Region::Position &new_position) {
	if (regions.count(region.position) == 0)
		return false;
	auto handler = regions.extract(region.position);
	handler.mapped()->position = new_position;
	if (&region != handler.mapped().get())
		region.position = new_position;
	handler.key() = new_position;
	regions.insert(std::move(handler));
	return true;
}

Region::Position Game::suggestPosition(bool is_free, s64 x, s64 y) {
	enum Direction {Up, Right, Down, Left};
	Direction direction = Up;
	int max_extent = 1;
	int extent = 0;
	for (;;) {
		if (regions.count({x, y}) == (is_free? 0 : 1))
			return {x, y};

		switch (direction) {
			case Up:    --y; break;
			case Right: ++x; break;
			case Down:  ++y; break;
			case Left:  --x; break;
		}

		if (++extent == max_extent) {
			extent = 0;
			switch (direction) {
				case Up:    direction = Right; break;
				case Right: direction = Down; ++max_extent; break;
				case Down:  direction = Left;  break;
				case Left:  direction = Up;   ++max_extent; break;
			}
		}
	}
}

bool Game::updateName(Region &region, const std::string &new_name) {
	if (new_name.find_first_of(Region::INVALID_CHARS) != std::string::npos)
		throw std::invalid_argument("Invalid region name.");
	region.name = new_name;
	return true;
}

Region & Game::currentRegion() {
	return *regions.at(position);
}

bool Game::erase(Region &region) {
	// If this region is the only region, don't erase it.
	if (regions.size() == 1 && regions.begin()->second->position == region.position)
		return false;
	const bool reposition = region.position == position;
	for (auto iter = extractions.begin(); iter != extractions.end();) {
		const Extraction &extraction = *iter;
		if (extraction.area->parent->position == region.position)
			extractions.erase(iter++);
		else
			++iter;
	}
	const bool out = regions.erase(region.position) != 0;
	if (reposition)
		position = suggestPosition(false, position.first, position.second);
	return out;
}

void Game::eraseExtractions(const Area &area) {
	for (auto iter = extractions.begin(); iter != extractions.end();) {
		const Extraction &extraction = *iter;
		if (extraction.area->name == area.name && extraction.area->parent == area.parent)
			extractions.erase(iter++);
		else
			++iter;
	}
}

void Game::tick(double delta) {
	for (auto &pair: regions)
		pair.second->tick(delta);
	for (auto iter = extractions.begin(); iter != extractions.end();) {
		Extraction &extraction = *iter;
		double &in_inventory = inventory[extraction.resourceName];
		double &in_region = extraction.area->resources[extraction.resourceName];
		if (extraction.startAmount == 0) { // Eternal extraction
			const double to_extract = std::min(in_region, extraction.rate * delta);
			in_region -= to_extract;
			in_inventory += to_extract;
			++iter;
		} else {
			const double to_extract = std::min(extraction.rate * delta, extraction.amount);
			if (in_region <= to_extract) {
				in_inventory += in_region;
				const double floored = std::floor(in_inventory);
				if (1 - (in_inventory - floored) < Resource::MIN_AMOUNT)
					in_inventory = floored + 1;
				extraction.area->resources.erase(extraction.resourceName);
				extractions.erase(iter++);
			} else {
				in_inventory += to_extract;
				in_region -= to_extract;
				extraction.amount -= to_extract;
				if (extraction.amount < Resource::MIN_AMOUNT) {
					const double floored = std::floor(in_inventory);
					if (1 - (in_inventory - floored) < Resource::MIN_AMOUNT)
						in_inventory = floored + 1;
					extractions.erase(iter++);
				} else
					++iter;
			}
		}
	}
	for (auto &processor: processors)
		processor->tick(delta);
	for (auto &link: automationLinks)
		link.tick();
}

void Game::loadDefaults() {
	regions.clear();
	Region &home = *regions.insert({{0, 0}, std::make_unique<Region>(*this, NameGen::makeRandomLanguage().makeName(), Region::Position(0, 0), 128)}).first->second;
	home.greed = 0.25;
	home.money = 10'000;
	auto forest = std::make_shared<ForestArea>(&home, 32);
	auto housing = std::make_shared<HousingArea>(&home, 20);
	auto mountain = std::make_shared<MountainArea>(&home, 48);
	auto lake = std::make_shared<LakeArea>(&home, 8);
	auto farmland = std::make_shared<FarmlandArea>(&home, 20);
	forest->setName("Forest").setPlayerOwned(true);
	housing->setName("Small Town");
	mountain->setName("Mountain").setPlayerOwned(true);
	lake->setName("Lake").setPlayerOwned(true);
	farmland->setName("Farm").setPlayerOwned(false);
	home += forest;
	home += housing;
	home += mountain;
	home += lake;
	home += farmland;
	processors.push_back(std::make_shared<Furnace>(*this));
	Logger::info("Loaded default data.");
}

void Game::extract(Area &area, const std::string &name, double amount) {
	extractions.emplace_back(&area, name, amount, resources.at(name).defaultExtractionRate);
}

void Game::extract(Area &area, const std::string &name, double amount, double rate) {
	extractions.emplace_back(&area, name, amount, rate);
}

std::string Game::toString() const {
	std::stringstream out;
	out << "[Regions]\n";
	for (const auto &pair: regions)
		out << pair.second->toString() << "\n";
	out << "\n[Inventory]\n";
	out << "money=" << money << "\n";
	for (const auto &[name, amount]: inventory)
		out << name << "=" << amount << "\n";
	out << "\n[CraftingInventory]\n";
	for (const auto &[name, amount]: craftingInventory)
		out << name << "=" << amount << "\n";
	out << "\n[Position]\n";
	out << position.first << "," << position.second << "\n";
	out << "\n[Extractions]\n";
	for (const Extraction &extraction: extractions)
		out << extraction.toString() << "\n";
	out << "\n[Processors]\n";
	for (const std::shared_ptr<Processor> &processor: processors)
		out << processor->toString() << "\n";
	out << "\n[Automations]\n";
	for (const AutomationLink &link: automationLinks)
		out << link.toString() << "\n";
	return out.str();
}

std::shared_ptr<Game> Game::fromString(const std::string &str) {
	std::vector<std::string> lines = split(str, "\n", true);
	/** Compliant saves must follow this order beginning at Regions. */
	enum class Mode {None, Regions, Inventory, CraftingInventory, Position, Extractions, Processors, Automations};
	Mode mode = Mode::None;

	std::shared_ptr<Game> out = std::make_shared<Game>();

	for (std::string &line: lines) {
		if (line.empty() || line == "\r")
			continue;
		if (line.back() == '\r')
			line.pop_back();
		if (line[0] == '[') {
			if (line == "[Regions]")
				mode = Mode::Regions;
			else if (line == "[Inventory]")
				mode = Mode::Inventory;
			else if (line == "[CraftingInventory]")
				mode = Mode::CraftingInventory;
			else if (line == "[Position]")
				mode = Mode::Position;
			else if (line == "[Extractions]")
				mode = Mode::Extractions;
			else if (line == "[Processors]")
				mode = Mode::Processors;
			else if (line == "[Automations]")
				mode = Mode::Automations;
			else {
				Logger::error("Invalid line: \"%s\"", line.c_str());
				for (char ch: line)
					Logger::info("%d / '%c'", ch, ch);
				throw std::invalid_argument("Invalid line");
			}
		} else {
			switch (mode) {
				case Mode::Regions: {
					auto region = Region::fromString(*out, line);
					out->regions.emplace(region->position, std::move(region));
					break;
				}
				case Mode::Inventory:
				case Mode::CraftingInventory: {
					const size_t equals = line.find('=');
					if (equals == std::string::npos)
						throw std::invalid_argument("Invalid Inventory line");
					const std::string type = line.substr(0, equals);
					const double amount = parseDouble(line.substr(equals + 1));
					if (type == "money")
						out->money = amount;
					else
						(mode == Mode::CraftingInventory? out->craftingInventory : out->inventory).emplace(type, amount);
					break;
				}
				case Mode::Position: {
					const size_t comma = line.find(',');
					if (comma == std::string::npos)
						throw std::invalid_argument("Invalid Position line");
					out->position = {parseLong(line.substr(0, comma)), parseLong(line.substr(comma + 1))};
					break;
				}
				case Mode::Extractions:
					out->extractions.push_back(Extraction::fromString(*out, line));
					break;
				case Mode::Processors: {
					std::shared_ptr<Processor> processor(Processor::fromString(*out, line));
					out->processors.push_back(processor);
					out->processorsByID.emplace(processor->id, processor);
					break;
				}
				case Mode::Automations:
					out->automationLinks.emplace_back(*out, line);
					out->automationLinks.back().setup();
					break;
				default: throw std::runtime_error("Invalid mode: " + std::to_string(static_cast<int>(mode)));
			}
		}
	}

	return out;
}

std::shared_ptr<Game> Game::load() {
	if (!FS::fileExists("/switch/TradeGame/save.txt"))
		throw std::runtime_error("Save data doesn't exist");
	return fromString(FS::readFile("/switch/TradeGame/save.txt"));
}

void Game::save() {
	Result result = 0;
	if (!FS::dirExists("/switch")) {
		if (R_FAILED(result = fsFsCreateDirectory(&fs, "/switch")))
			throw std::runtime_error("fsFsCreateDirectory(/switch) failed: 0x" + hex(result));
	}

	if (!FS::dirExists("/switch/TradeGame")) {
		if (R_FAILED(result = fsFsCreateDirectory(&fs, "/switch/TradeGame")))
			throw std::runtime_error("fsFsCreateDirectory(/switch/TradeGame) failed: 0x" + hex(result));
	}

	FS::writeFile("/switch/TradeGame/save.txt", toString());
}

Game & Game::operator+=(std::unique_ptr<Region> &&ptr) {
	if (regions.count(ptr->position) != 0)
		throw std::runtime_error("A region already exists at (" + std::to_string(ptr->position.first) + ", "
			+ std::to_string(ptr->position.first) + ")");
	regions.emplace(ptr->position, std::move(ptr));
	return *this;
}

Extraction * Game::getExtraction(const Area &area, const std::string &name) {
	for (Extraction &extraction: extractions)
		if (extraction.area == &area && extraction.resourceName == name)
			return &extraction;
	return nullptr;
}

const Extraction * Game::getExtraction(const Area &area, const std::string &name) const {
	for (const Extraction &extraction: extractions)
		if (extraction.area == &area && extraction.resourceName == name)
			return &extraction;
	return nullptr;
}
