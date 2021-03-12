#include "Processor.h"

struct Centrifuge: public Processor {
	using Processor::Processor;
	Type getType() const override { return Type::Centrifuge; }
};
