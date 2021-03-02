#pragma once

#include <switch/runtime/devices/console.h>

namespace Logger {
	template <typename ...Args>
	void info(Args &&...args) {
		printf("\e[36m [info] \e[39m");
		printf(std::forward<Args>(args)...);
		consoleUpdate(nullptr);
	}

	template <typename ...Args>
	void error(Args &&...args) {
		printf("\e[31m[error] \e[39m");
		printf(std::forward<Args>(args)...);
		consoleUpdate(nullptr);
	}

	template <typename ...Args>
	void warn(Args &&...args) {
		printf("\e[33m [warn] \e[39m");
		printf(std::forward<Args>(args)...);
		consoleUpdate(nullptr);
	}
}
