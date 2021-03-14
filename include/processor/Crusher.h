#include "Processor.h"

struct Crusher: public Processor {
	using Processor::Processor;
	double tick(double delta) override;
	Type getType() const override { return Type::Crusher; }
	void headerButtons(Context &, long index) override;
};
