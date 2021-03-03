#pragma once

#include <cstddef>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

class Game;

class Resource {
	public:
		using Name = std::string;
		using Type = std::string;
		using Map  = std::unordered_map<Name, double>;

		struct Conversion {
			double amount; // From converting 1 unit of the source resource
			std::vector<std::string> tags; // Conversion is valid if the converter has any of these tags.
			Conversion() = delete;
			Conversion(double amount_): amount(amount_) {}
			Conversion(double amount_, const std::vector<std::string> &tags_): amount(amount_), tags(tags_) {}
		};

		Game *owner;
		Name name;
		std::set<Type> types;
		std::unordered_multimap<Name, Conversion> conversions;

		Resource(Game *, const Name &);
		Resource(Game *, const char *);

		template <typename... Args>
		Resource & addTypes(Args &&...args) {
			(types.insert(args), ...);
			return *this;
		}

		Resource & add(const Name &, const Conversion &);

		bool hasType(const Type &) const;
		static bool hasType(Game &, const Name &, const Type &);

		bool operator==(const Resource &other) { return name == other.name; }
		bool operator!=(const Resource &other) { return name != other.name; }
		bool operator< (const Resource &other) { return name <  other.name; }
};

