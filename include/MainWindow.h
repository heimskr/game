#pragma once

#include <memory>

class Context;
class Area;

class MainWindow {
	private:
		bool insert(std::shared_ptr<Area>, const std::string &, double);

	public:
		constexpr static int TAB_COUNT = 5;
		Context &context;
		int selectedTab = 0;
		int lastTab = 0;

		MainWindow() = delete;
		MainWindow(Context &context_): context(context_) {}

		void render(bool *open);
};
