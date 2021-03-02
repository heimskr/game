#pragma once

#include <sys/types.h>

enum class State {Invalid, Initial};

void perish();
void cleanup();
time_t getTime();
