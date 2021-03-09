#pragma once

#include <list>
#include <map>
#include <memory>

#include "Extraction.h"
#include "Region.h"
#include "Resource.h"
#include "Util.h"

class Game {
	public:
		bool ready = false;
		std::map<std::string, Resource> resources;
		std::map<std::string, double> inventory;
		std::map<Region::Position, std::unique_ptr<Region>> regions;
		std::list<Extraction> extractions;
		Region::Position position;

		Game();

		void add(const Resource &);
		void addResources();
		void addResource(const std::string &);
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

		Game & operator+=(std::unique_ptr<Region> &&);
		Extraction * getExtraction(const Area &, const std::string &);
		const Extraction * getExtraction(const Area &, const std::string &) const;
};
