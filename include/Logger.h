#pragma once

#include <stdio.h>
#include <switch/runtime/devices/console.h>

extern PrintConsole *console;

namespace Logger {
	template <typename ...Args>
	void info(Args &&...args) {
		printf("\e[2m[\e[22;36mi\e[39;2m]\e[22m \e[39m");
		printf(std::forward<Args>(args)...);
		printf("\n");
		consoleUpdate(console);
	}

	template <typename ...Args>
	void error(Args &&...args) {
		printf("\e[2m[\e[22;31m!\e[39;2m]\e[22m \e[39m");
		printf(std::forward<Args>(args)...);
		printf("\n");
		consoleUpdate(console);
	}

	template <typename ...Args>
	void warn(Args &&...args) {
		printf("\e[2m[\e[22;33m!\e[39;2m]\e[22m \e[39m");
		printf(std::forward<Args>(args)...);
		printf("\n");
		consoleUpdate(console);
	}
}
