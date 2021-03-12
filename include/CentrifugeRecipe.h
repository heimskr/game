#pragma once

#include <list>
#include <string>
#include <utility>

struct CentrifugeOutput {
	std::string name;
	double amount;
	double chance;
	CentrifugeOutput(const std::string &name_, double amount_, double chance_);
};

class CentrifugeRecipe {
	public:
		std::string inputResource;
		const std::list<CentrifugeOutput> outputs;
		const double probabilitySum;

		CentrifugeRecipe(const std::string &input_resource, const std::list<CentrifugeOutput> &outputs_);

		// double probabilitySum() const;
		std::pair<std::string, double> choose() const;

	private:
		double getSum(const decltype(outputs) &) const;
};
