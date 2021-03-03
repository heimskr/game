#pragma once

#include <stdio.h>
#include <switch/runtime/devices/console.h>

extern PrintConsole *console;

namespace Logger {
	template <typename ...Args>
	void info(Args &&...args) {
		printf("\e[36m [info] \e[39m");
		printf(std::forward<Args>(args)...);
		printf("\n");
		consoleUpdate(console);
	}

	template <typename ...Args>
	void error(Args &&...args) {
		printf("\e[31m[error] \e[39m");
		printf(std::forward<Args>(args)...);
		printf("\n");
		consoleUpdate(console);
	}

	template <typename ...Args>
	void warn(Args &&...args) {
		printf("\e[33m [warn] \e[39m");
		printf(std::forward<Args>(args)...);
		printf("\n");
		consoleUpdate(console);
	}
}
