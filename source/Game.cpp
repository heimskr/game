#include <string>
#include <utility>

#include "Game.h"

namespace Globals {
	std::map<Resource::Name, Resource> resources;

	void add(const Resource &resource) {
		resources.emplace(resource.name, resource);
	}

	void addResources() {
		add(Resource("charcoal").addTypes("fuel"));
		add(Resource("coal").addTypes("fuel"));
		add(Resource("wood").addTypes("fuel", "natural").add("charcoal", {1, {"furnace"}}));
		add(Resource("water").addTypes("liquid"));
		add(Resource("ironOre").addTypes("ore", "iron").add("iron", {0.5, {"furnace"}}));
		add(Resource("iron").addTypes("metal", "iron"));
		add(Resource("human").addTypes("alive", "sapient"));
	}

	void addAll() {
		addResources();
	}
}

void Game::init() {
	Globals::addAll();
}
