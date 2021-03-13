#include "Processor.h"

struct Fermenter: public Processor {
	double yeast = 0.;

	Fermenter(Game &, double fuel_, const std::map<std::string, double> &input_, const std::map<std::string, double> &output_, bool auto_extract);
	Fermenter(Game &);

	virtual std::string toString() const override;
	double tick(double delta) override;
	Type getType() const override { return Type::Fermenter; }
};
