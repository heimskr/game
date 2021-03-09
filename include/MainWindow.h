#pragma once

struct Context;

struct MainWindow {
	constexpr static int TAB_COUNT = 4;
	Context &context;
	int selectedTab = 0;
	int lastTab = 0;

	MainWindow() = delete;
	MainWindow(Context &context_): context(context_) {}

	void render(bool *open);
};
