#pragma once

#include <string>

class Region;

/** Base prices go through multiple stages of modification before they're used in the market. This namespace contains
 *  functions to transform prices through various steps of modification. A buy price is the amount of money the player
 *  has to pay for a resource, while the sell price is how much money the player can receive in exchange for it. */
namespace Stonks {
	constexpr double E = 2.71828182845904523536;

	/** Returns the price for which a region will buy a resource, given the resource's sell price. */
	double buyPriceToSellPrice(double buy_price, double greed);

	/** Prices depend on the amount of money a region has. This adjusts a price accordingly. */
	double applyMoney(double base_price, size_t region_money);

	/** Prices depend on the amount of the resource a region owns. This adjusts a price accordingly. */
	double applyScarcity(double base_price, double resource_amount);

	/** Determines the buy price for a single unit of a resource. */
	double buyPrice(double base_price, double resource_amount, size_t region_money);

	/** Determines the sell price for a single unit of a resource. */
	double sellPrice(double base_price, double resource_amount, size_t region_money, double greed);

	/** Determines whether the region can afford a given amount of a certain resource, and if so, outputs the price.
	 *  Unfortunately, this is O(amount). */
	bool totalSellPrice(const Region &region, const std::string &resource_name, double amount, size_t &out);

	/** Returns the price to buy a given amount of a resource from a region. Like totalSellPrice, this is O(amount). */
	size_t totalBuyPrice(const Region &region, const std::string &resource_name, double amount);
}
