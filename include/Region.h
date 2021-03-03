#pragma once

#include <set>
#include <stddef.h>
#include <string>
#include <switch/types.h>
#include <utility>

#include "Area.h"
#include "Resource.h"

class Game;

/** Regions are individual locations within the broader game universe. It's yet to be determined whether they're
 *  islands, planets or something else. Regions are divided into Areas. A Region can have only one Area of a given type.
 */
class Region {
	public:
		static constexpr const char *INVALID_CHARS = ";";
		using Position = std::pair<s64, s64>;

		Game *owner;
		std::string name;
		Position position;
		size_t size = 0;
		/** Don't directly insert Areas into this map; use operator+= instead. */
		std::unordered_map<std::string, Area> areas;
		size_t money = 0;

		Region(Game *, const std::string &name_, const Position &, size_t size_);

		Resource::Map allResources() const;
		size_t totalPopulation() const;

		Region & setSize(size_t);
		Region & setMoney(size_t);
		Region & setPosition(const std::pair<s64, s64> &);
		Region & setName(const std::string &);

		Region & operator+=(Area);

		std::string toString() const;
};
