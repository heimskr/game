#include "Processor.h"

struct Electrolyzer: public Processor {
	using Processor::Processor;
	double tick(double delta) override;
	Type getType() const override { return Type::Electrolyzer; }
	// void headerButtons(Context &, long index) override;
};
