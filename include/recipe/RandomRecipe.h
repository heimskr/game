#pragma once

#include <string>
#include <utility>
#include <vector>

struct RandomOutput {
	std::string name;
	double amount;
	double chance;
	RandomOutput(const std::string &name_, double amount_, double chance_);
};

class RandomRecipe {
	public:
		const std::vector<RandomOutput> outputs;
		const double probabilitySum;

		RandomRecipe(const std::vector<RandomOutput> &outputs_);
		RandomRecipe(std::vector<RandomOutput> &&outputs_);

		std::pair<std::string, double> choose() const;

	protected:
		double getSum(const decltype(outputs) &) const;
};
