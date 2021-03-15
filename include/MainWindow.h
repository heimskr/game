#pragma once

#include <memory>

class Area;
class Context;
class Region;

class MainWindow {
	private:
		bool insert(std::shared_ptr<Area>, const std::string &, double);

	public:
		constexpr static int TAB_COUNT = 8;
		Context &context;
		int selectedTab = 0;
		int lastTab = 0;

		std::map<std::string, double> craftingOutput;

		MainWindow() = delete;
		MainWindow(Context &context_): context(context_) {}

		void render(bool *open);
		void renderRegion(Region *);
		void renderTravel(Region *);
		void renderExtractions();
		void renderInventory();
		void renderConversion();
		void renderMarket(Region *);
		void renderAutomation();
		void renderCrafting();
		void computeCraftingOutput();
};
