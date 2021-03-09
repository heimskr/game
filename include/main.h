#pragma once

#include <memory>
#include <string>

class Game;

struct Context {
	std::shared_ptr<Game> game;

	bool showExtractWindow = false;
	bool showLoadWindow = false;

	std::string message;

	void save();
};
