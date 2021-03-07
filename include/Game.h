#pragma once

#include <map>
#include <memory>

#include "Region.h"
#include "Resource.h"
#include "Util.h"

class Game {
	public:
		bool ready = false;
		std::map<Resource::Name, Resource> resources;
		std::map<Resource::Name, double> inventory;
		std::map<Region::Position, Region> regions;
		Region::Position position;

		Game();

		void add(const Resource &);
		void addResources();
		void addResource(const Resource::Name &);
		void addAll();

		bool updatePosition(Region &, const Region::Position &);
		Region::Position suggestPosition();
		bool updateName(Region &, const std::string &);

		void listRegions();
		Region * addRegion();
		Region & currentRegion();

		void tick();

		void loadDefaults();

		std::string toString() const;
		static std::shared_ptr<Game> fromString(const std::string &);
		static std::shared_ptr<Game> load();
		void save();
};