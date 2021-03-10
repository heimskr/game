#include <sstream>
#include <stdexcept>

#include "Area.h"
#include "Region.h"
#include "area/Areas.h"
#include "Util.h"

Area::Area(Region *parent_, size_t size_): parent(parent_), size(size_) {}

Area & Area::setResources(const Resource::Map &resources_) {
	resources = resources_;
	return *this;
}

Area & Area::setSize(size_t size_) {
	size = size_;
	return *this;
}

Area & Area::setPlayerOwned(bool player_owned) {
	playerOwned = player_owned;
	return *this;
}

Area & Area::setName(const std::string &name_) {
	if (name_.find_first_of(INVALID_CHARS) != std::string::npos)
		throw std::invalid_argument("Invalid area name");
	name = name_;
	return *this;
}

size_t Area::totalPopulation() const {
	size_t out = 0;
	for (const auto &pair: resources)
		if (Resource::hasType(*parent->owner, pair.first, "sapient"))
			out += static_cast<size_t>(pair.second);
	return out;
}

std::string Area::toString() const {
	std::stringstream out;
	out << name << ":" << size << ":" << (playerOwned? "1" : "0") << ":" << static_cast<unsigned>(getType()) << ":";
	bool first = true;
	for (const auto &pair: resources) {
		if (first)
			first = false;
		else
			out << "/";
		out << pair.first << "/" << pair.second;
	}
	return out.str();
}

std::shared_ptr<Area> Area::fromString(Region &region, const std::string &str) {
	const std::vector<std::string> pieces = split(str, ":", false);
	if (pieces.size() < 4)
		throw std::runtime_error("Invalid Area string");
	const std::string &name = pieces[0];
	const size_t size = parseLong(pieces[1]);
	const bool player_owned = pieces[2] == "1";
	const Type type = static_cast<Type>(parseLong(pieces[3]));
	Resource::Map resources;
	const std::vector<std::string> by_slash = split(pieces[4], "/", false);
	for (size_t i = 0; i < by_slash.size(); i += 2)
		resources.emplace(by_slash[i], parseDouble(by_slash[i + 1]));
	std::shared_ptr<Area> area;
	switch (type) {
		case Type::Housing:  area = std::make_shared<HousingArea>(&region);  break;
		case Type::Forest:   area = std::make_shared<ForestArea>(&region);   break;
		case Type::Mountain: area = std::make_shared<MountainArea>(&region); break;
		case Type::Lake:     area = std::make_shared<LakeArea>(&region);     break;
		case Type::Empty:    area = std::make_shared<EmptyArea>(&region);    break;
		case Type::Farmland: area = std::make_shared<FarmlandArea>(&region, 0, pieces[5]); break;
		default: throw std::invalid_argument("Unknown Area type: " + std::to_string(static_cast<unsigned>(type)));
	}
	area->setName(name).setSize(size).setPlayerOwned(player_owned).setResources(resources);
	return area;
}
