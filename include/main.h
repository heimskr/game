#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <sys/types.h>
#include <vector>

class Game;

std::chrono::milliseconds getTime();

class Context {
	public:
		std::shared_ptr<Game> game;
		bool loaded = false;

		bool showResourcePicker = false;
		std::function<void(const std::string &)> onResourcePicked = [](const std::string &) {};

		std::string message;
		bool isConfirm = false;
		std::function<void(bool)> onChoice = [](bool) {};

		void newGame();
		void load();
		void save();

		void pickResource(std::function<void(const std::string &)>);
		void confirm(const std::string &, std::function<void(bool)>);
		void showMessage(const std::string &);

		// Each frame, every function in this vector is called after the UI is rendered. The vector is then cleared.
		std::vector<std::function<void()>> frameActions;

		Game * operator->() {
			return game.get();
		}

		const Game * operator->() const {
			return game.get();
		}
};
