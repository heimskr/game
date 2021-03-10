#include <cmath>

#include "Region.h"
#include "Stonks.h"

namespace Stonks {
	double buyPriceToSellPrice(double buy_price, const Region &region) {
		return buy_price / (1. + region.greed);
	}

	double applyMoney(double base_price, const Region &region) {
		return base_price / (1. + (1. / pow(E, -region.money / 50.)));
	}

	double applyScarcity(double base_price, const std::string &resource_name, const Region &region) {
		return base_price / (1. + (1. / pow(E, -region.allNonOwnedResources()[resource_name] / 100.)));
	}

	double buyPrice(double base_price, const std::string &resource_name, const Region &region) {
		return applyScarcity(applyMoney(base_price, region), resource_name, region);
	}

	double sellPrice(double base_price, const std::string &resource_name, const Region &region) {
		return buyPriceToSellPrice(buyPrice(base_price, resource_name, region), region);
	}
}
