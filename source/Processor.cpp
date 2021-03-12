#include "Processor.h"
#include "Util.h"
#include "processor/Processors.h"

Processor::Processor(Type type_, const Resource::Map &input_, const Resource::Map &output_):
	type(type_), input(input_), output(output_) {}

std::string Processor::toString() const {
	return std::to_string(static_cast<int>(type)) + ":" + stringify(input) + ":" + stringify(output);
}

Processor * Processor::fromString(const std::string &str) {
	const std::vector<std::string> pieces = split(str, ":", false);
	const Type type = static_cast<Type>(parseLong(pieces[0]));
	Resource::Map input(parseMap(pieces[1])), output(parseMap(pieces[2]));
	switch (type) {
		case Type::Furnace:    return new Furnace(type, std::move(input), std::move(output));
		case Type::Centrifuge: return new Centrifuge(type, std::move(input), std::move(output));
		default: return nullptr;
	}
}
