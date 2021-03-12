#pragma once

#include <list>
#include <map>
#include <memory>

#include "Extraction.h"
#include "Region.h"
#include "Resource.h"
#include "Util.h"

class Processor;

class Game {
	public:
		bool ready = false;
		std::map<std::string, Resource> resources;
		std::map<Processor::Type, Resource::Map> processorCosts;

		std::map<std::string, double> inventory;
		std::map<Region::Position, std::unique_ptr<Region>> regions;
		std::list<Extraction> extractions;
		std::list<std::unique_ptr<Processor>> processors;
		Region::Position position;
		size_t money = 0;
		bool cheatsEnabled = false;

		Game();

		void add(const Resource &);
		void addResources();
		void add(Processor::Type, const Resource::Map &);
		void addProcessorCosts();
		void addAll();
		std::string randomResource(const Resource::Type &) const;

		bool updatePosition(Region &, const Region::Position &);
		Region::Position suggestPosition(bool is_free, s64 x = 0, s64 y = 0);
		bool updateName(Region &, const std::string &);

		void listRegions();
		Region * addRegion();
		Region & currentRegion();
		bool erase(Region &);
		void eraseExtractions(const Area &);

		void tick(double delta);

		void loadDefaults();

		void extract(Area &, const std::string &name, double amount);
		void extract(Area &, const std::string &name, double amount, double rate);

		std::string toString() const;
		static std::shared_ptr<Game> fromString(const std::string &);
		static std::shared_ptr<Game> load();
		void save();

		Game & operator+=(std::unique_ptr<Region> &&);
		Extraction * getExtraction(const Area &, const std::string &);
		const Extraction * getExtraction(const Area &, const std::string &) const;
};
