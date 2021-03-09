#pragma once

#include <memory>
#include <set>
#include <stddef.h>
#include <string>
#include <switch/types.h>
#include <unordered_set>
#include <utility>

#include "Area.h"
#include "Direction.h"
#include "Resource.h"

class Game;

/** Regions are individual locations within the broader game universe. It's yet to be determined whether they're
 *  islands, planets or something else. Regions are divided into Areas. A Region can have only one Area of a given type.
 */
class Region {
	public:
		static constexpr const char *INVALID_CHARS = ":;";
		using Position = std::pair<s64, s64>;

		Game *owner;
		std::string name;
		Position position;
		size_t size = 0;
		/** Don't directly insert Areas into this map; use operator+= instead. */
		std::unordered_map<std::string, std::shared_ptr<Area>> areas;
		size_t money = 0;

		Region(Game *, const std::string &name_, const Position &, size_t size_);

		void tick();

		Resource::Map allResources() const;
		size_t totalPopulation() const;
		bool hasNeighbor() const;
		bool hasNeighbor(Direction) const;
		Region * getNeighbor(Direction) const;
		std::unordered_set<Direction> validDirections() const;

		bool updateName(Area &, const std::string &);

		Region & setSize(size_t);
		Region & setMoney(size_t);
		Region & setPosition(const std::pair<s64, s64> &);
		Region & setName(const std::string &);

		Region & operator+=(std::shared_ptr<Area>);

		std::string toString() const;
		static std::unique_ptr<Region> fromString(Game &, const std::string &);
};

Region::Position operator+(const Region::Position &, const Region::Position &);
