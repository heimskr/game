#include "Processor.h"

struct Refinery: public Processor {
	using Processor::Processor;
	double tick(double delta) override;
	Type getType() const override { return Type::Refinery; }
	void headerButtons(Context &, long index) override;
};
