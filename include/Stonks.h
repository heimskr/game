#pragma once

#include <string>

class Region;

/** Base prices go through multiple stages of modification before they're used in the market. This namespace contains
 *  functions to transform prices through various steps of modification. A buy price is the amount of money the player
 *  has to pay for a resource, while the sell price is how much money the player can receive in exchange for it. */
namespace Stonks {
	constexpr double E = 2.71828182845904523536;

	/** Returns the price for which a region will buy a resource, given the resource's sell price. */
	double buyPriceToSellPrice(double buy_price, const Region &);

	/** Prices depend on the amount of money a region has. This adjusts a price accordingly. */
	double applyMoney(double base_price, const Region &);

	/** Prices depend on the amount of the resource a region owns. This adjusts a price accordingly. */
	double applyScarcity(double base_price, const std::string &, const Region &);

	double buyPrice(double base_price, const std::string &, const Region &);

	double sellPrice(double base_price, const std::string &, const Region &);
}
