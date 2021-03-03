#pragma once

#include <map>

#include "Region.h"
#include "Resource.h"
#include "Util.h"

class Game {
	public:
		bool ready = false;
		std::map<Resource::Name, Resource> resources;
		std::map<Region::Position, Region> regions;

		Game();

		void add(const Resource &);
		void addResources();
		void addResource(const Resource::Name &);
		void addAll();

		bool updatePosition(Region &, const Region::Position &);
		Region::Position suggestPosition();

		void listRegions();
		Region * addRegion();

		void loadDefaults();

		std::string toString() const;
};