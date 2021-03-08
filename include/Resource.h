#pragma once

#include <cstddef>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

class Game;

class Resource {
	public:
		using Type = std::string;
		using Map  = std::unordered_map<std::string, double>;

		struct Conversion {
			double amount; // From converting 1 unit of the source resource
			std::vector<std::string> tags; // Conversion is valid if the converter has any of these tags.
			Conversion() = delete;
			Conversion(double amount_): amount(amount_) {}
			Conversion(double amount_, const std::vector<std::string> &tags_): amount(amount_), tags(tags_) {}
		};

		Game *owner;
		std::string name;
		std::set<Type> types;
		std::unordered_multimap<std::string, Conversion> conversions;
		bool discrete = false;
		double defaultExtractionRate = 1.0;

		Resource(Game *, const std::string &);
		Resource(Game *, const char *);

		template <typename... Args>
		Resource & addTypes(Args &&...args) {
			(types.insert(args), ...);
			return *this;
		}

		Resource & add(const std::string &, const Conversion &);

		bool hasType(const Type &) const;
		static bool hasType(Game &, const std::string &, const Type &);

		Resource & setDiscrete(bool);
		Resource & setDefaultExtractionRate(double);

		bool operator==(const Resource &other) { return name == other.name; }
		bool operator!=(const Resource &other) { return name != other.name; }
		bool operator< (const Resource &other) { return name <  other.name; }
};

