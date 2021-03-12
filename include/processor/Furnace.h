#include "Processor.h"

struct Furnace: public Processor {
	using Processor::Processor;
	void tick() override;
};
