#pragma once

#include <memory>
#include <string>

class Game;

struct Context {
	std::shared_ptr<Game> game;
	bool loaded = false;

	bool showExtractWindow = false;

	std::string message;

	void load();
	void save();
};
