#pragma once

#include <cstddef>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "Processor.h"

class Game;

class Resource {
	public:
		constexpr static double MIN_AMOUNT = 0.000001;

		using Type = std::string;
		using Map  = std::map<std::string, double>;

		struct Conversion {
			double amount; // From converting 1 unit of the source resource
			double rate;   // Amount of source resource converted per second
			std::string outName;
			Conversion() = delete;
			Conversion(double amount_, const std::string &out_name, double rate_ = 1.):
				amount(amount_), rate(rate_), outName(out_name) {}
		};

		Game *owner;
		std::string name;
		std::set<Type> types;
		std::unordered_map<Processor::Type, Conversion> conversions;
		bool discrete = false;
		double defaultExtractionRate = 1.0;
		double basePrice = 1.0;

		Resource(Game *, const std::string &);
		Resource(Game *, const char *);

		template <typename... Args>
		Resource & addTypes(Args &&...args) {
			(types.insert(args), ...);
			return *this;
		}

		Resource & add(Processor::Type, const Conversion &);

		bool hasType(const Type &) const;
		static bool hasType(Game &, const std::string &, const Type &);

		Resource & setDiscrete(bool);
		Resource & setDefaultExtractionRate(double);
		Resource & setBasePrice(double);

		bool operator==(const Resource &other) { return name == other.name; }
		bool operator!=(const Resource &other) { return name != other.name; }
		bool operator< (const Resource &other) { return name <  other.name; }
};
