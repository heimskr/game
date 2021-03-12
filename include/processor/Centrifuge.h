#include "Processor.h"

struct Centrifuge: public Processor {
	using Processor::Processor;
	void tick() override;
};
