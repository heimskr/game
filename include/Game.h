#pragma once

#include <map>

#include "Region.h"
#include "Resource.h"
#include "Util.h"

class Game {
	public:
		bool ready = false;
		std::map<Resource::Name, Resource> resources;
		std::map<decltype(Region::position), Region> regions;

		Game();

		void add(const Resource &);
		void addResources();
		void addResource(const Resource::Name &);
		void addAll();

		bool updatePosition(Region &, const decltype(Region::position) &);

		void listRegions();
		Region * addRegion();

		std::string toString() const;
};