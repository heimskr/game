#pragma once

#include <cstddef>
#include <memory>
#include <string>

#include "Resource.h"

class Region;

class Area {
	public:
		static constexpr const char *INVALID_CHARS = ";:/";

		enum class Type: unsigned {Housing, Forest, Mountain, Lake, Empty, Farmland};

		Region *parent;
		Resource::Map resources;
		size_t size;
		std::string name;
		bool playerOwned = false;

		Area() = delete;
		Area(Region *, size_t = 0);

		Area & setResources(const Resource::Map &);
		Area & setSize(size_t);
		Area & setPlayerOwned(bool);
		Area & setName(const std::string &);

		size_t totalPopulation() const;

		std::string toString() const;
		static std::shared_ptr<Area> fromString(Region &, const std::string &);

		virtual Type getType() const = 0;
		virtual std::string description() const = 0;
		virtual void tick(double delta) = 0;
};
