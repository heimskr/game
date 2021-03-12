#include "Processor.h"

struct Furnace: public Processor {
	using Processor::Processor;
	Type getType() const override { return Type::Furnace; }
};
