#pragma once

#include <functional>
#include <memory>
#include <string>
#include <sys/types.h>

class Game;

time_t getTime();

class Context {
	public:
		std::shared_ptr<Game> game;
		bool loaded = false;

		bool showResourcePicker = false;
		std::function<void(const std::string &)> onResourcePicked = [](const std::string &) {};

		std::string message;

		void load();
		void save();

		void pickResource(std::function<void(const std::string &)>);

		Game * operator->() {
			return game.get();
		}

		const Game * operator->() const {
			return game.get();
		}
};
