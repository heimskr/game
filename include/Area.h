#pragma once

#include <cstddef>
#include <memory>
#include <string>

#include "Resource.h"

class Region;

class Area {
	public:
		static constexpr const char *INVALID_CHARS = ";:/";

		enum class Type: unsigned {Housing, Forest, Mountain, Lake, Empty, Farmland, OilField};

		Region *parent;
		Resource::Map resources;
		size_t size;
		std::string name;
		bool playerOwned = false;

		Area() = delete;
		Area(Region *, size_t = 0);

		virtual ~Area() {}

		Area & setResources(const Resource::Map &);
		Area & setResources(Resource::Map &&);
		Area & setSize(size_t);
		Area & setPlayerOwned(bool);
		Area & setName(const std::string &);
		Area & setName(std::string &&);

		bool reduceSize(size_t new_size);

		size_t totalPopulation() const;

		virtual std::string toString() const;
		static std::shared_ptr<Area> fromString(Region &, const std::string &);

		virtual Type getType() const = 0;
		virtual std::string description() const = 0;
		virtual void tick(double delta) = 0;
};
