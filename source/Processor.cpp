#include "Processor.h"
#include "Resource.h"
#include "Util.h"
#include "processor/Processors.h"
#include "Game.h"

Processor::Processor(Game &owner_, const Resource::Map &input_, const Resource::Map &output_):
	owner(&owner_), input(input_), output(output_) {}

Processor::Processor(Game &owner_): Processor(owner_, {}, {}) {}

std::string Processor::toString() const {
	return std::to_string(static_cast<int>(getType())) + ":" + stringify(input) + ":" + stringify(output);
}

Processor * Processor::fromString(Game &owner, const std::string &str) {
	const std::vector<std::string> pieces = split(str, ":", false);
	Resource::Map input(parseMap(pieces[1])), output(parseMap(pieces[2]));
	const Type type = static_cast<Type>(parseLong(pieces[0]));
	switch (type) {
		case Type::Furnace:    return new Furnace(owner, std::move(input), std::move(output));
		case Type::Centrifuge: return new Centrifuge(owner, std::move(input), std::move(output));
		default: throw std::invalid_argument("Invalid Processor type: " + std::to_string(static_cast<int>(type)));
	}
}

Processor * Processor::ofType(Game &owner, Type type) {
	switch (type) {
		case Type::Furnace:    return new Furnace(owner);
		case Type::Centrifuge: return new Centrifuge(owner);
		default: throw std::invalid_argument("Invalid Processor type: " + std::to_string(static_cast<int>(type)));
	}
}

double Processor::tick() {
	std::vector<const std::string *> to_remove;
	to_remove.clear();
	to_remove.reserve(input.size());
	double out = 0.;

	for (auto &[name, amount]: input) {
		const Resource &resource = owner->resources.at(name);
		if (resource.conversions.count(Type::Furnace) == 0)
			continue;
		const auto &conversion = resource.conversions.at(Type::Furnace);
		const double to_convert = std::min(amount, conversion.rate);
		if ((amount -= to_convert) < Resource::MIN_AMOUNT)
			to_remove.push_back(&name);
		out += to_convert;
		output[conversion.outName] += to_convert * conversion.amount;
	}

	for (const std::string *name: to_remove)
		input.erase(*name);

	return out;
}

const char * Processor::typeName(Type type) {
	switch (type) {
		case Type::Furnace:    return "Furnace";
		case Type::Centrifuge: return "Centrifuge";
		default: return "?";
	}
}
