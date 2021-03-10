#include <cmath>

#include "Game.h"
#include "Logger.h"
#include "Region.h"
#include "Stonks.h"

namespace Stonks {
	double buyPriceToSellPrice(double buy_price, double greed) {
		return buy_price / (1. + greed);
	}

	double applyMoney(double base_price, size_t region_money) {
		return base_price / (1. + (1. / pow(E, -region_money / 50.)));
	}

	double applyScarcity(double base_price, double resource_amount) {
		return base_price / (1. + (1. / pow(E, -resource_amount / 100.)));
	}

	double buyPrice(double base_price, double resource_amount, size_t region_money) {
		return applyScarcity(applyMoney(base_price, region_money), resource_amount);
	}

	double sellPrice(double base_price, double resource_amount, size_t region_money, double greed) {
		return buyPriceToSellPrice(buyPrice(base_price, resource_amount, region_money), greed);
	}

	bool totalSellPrice(const Region &region, const std::string &resource_name, double amount, size_t &out) {
		const Resource::Map non_owned = region.allNonOwnedResources();
		double region_amount = non_owned.count(resource_name)? non_owned.at(resource_name) : 0.;
		const double base = region.owner->resources.at(resource_name).basePrice;
		double price = 0.;
		double region_money = region.money;
		const double greed = region.greed;
		while (1. <= amount) {
			const double unit_price = sellPrice(base, region_amount++, region_money, greed);
			if (region_money < unit_price)
				return false;
			region_money -= unit_price;
			price += unit_price;
			--amount;
		}

		if (0. < amount) {
			const double subunit_price = amount * sellPrice(base, region_amount, region_money, greed);
			if (region_money < subunit_price)
				return false;
			region_money -= subunit_price;
			price += subunit_price;
		}

		const size_t discrete_price = std::ceil(price);

		if (region.money < discrete_price)
			return false;

		out = discrete_price;
		return true;
	}

	size_t totalBuyPrice(const Region &region, const std::string &resource_name, double amount) {
		const Resource::Map non_owned = region.allNonOwnedResources();
		double region_amount = non_owned.count(resource_name)? non_owned.at(resource_name) : 0.;
		const double base = region.owner->resources.at(resource_name).basePrice;
		double price = 0.;
		double region_money = region.money;
		while (1. <= amount) {
			const double unit_price = buyPrice(base, region_amount--, region_money);
			region_money += unit_price;
			price += unit_price;
			--amount;
		}

		if (0. < amount) {
			const double subunit_price = amount * buyPrice(base, region_amount, region_money);
			region_money += subunit_price;
			price += subunit_price;
		}

		// It's assumed the caller will check whether the player has enough money.
		return std::floor(price);
	}
}
