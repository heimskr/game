#pragma once

#include <memory>
#include <string>
#include <sys/types.h>

class Game;

time_t getTime();

struct Context {
	std::shared_ptr<Game> game;
	bool loaded = false;

	bool showResourcePicker = false;

	std::string message;

	void load();
	void save();

	Game * operator->() {
		return game.get();
	}

	const Game * operator->() const {
		return game.get();
	}
};
