#include <cmath>

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
		return buyPriceToSellPrice(buyPrice(base_price, resource_amount, greed), region_money);
	}
}
