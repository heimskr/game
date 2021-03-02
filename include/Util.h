#pragma once

#include <utility>
#include <switch.h>

#include "Logger.h"

template <typename ...Args>
void print(Args &&...args) {
	printf(std::forward<Args>(args)...);
	consoleUpdate(nullptr);
}
