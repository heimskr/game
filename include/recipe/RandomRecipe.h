#pragma once

#include <list>
#include <string>
#include <utility>

struct RandomOutput {
	std::string name;
	double amount;
	double chance;
	RandomOutput(const std::string &name_, double amount_, double chance_);
};

class RandomRecipe {
	public:
		const std::list<RandomOutput> outputs;
		const double probabilitySum;

		RandomRecipe(const std::list<RandomOutput> &outputs_);

		std::pair<std::string, double> choose() const;

	protected:
		double getSum(const decltype(outputs) &) const;
};
