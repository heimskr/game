#pragma once

#include <sys/types.h>

#include "Util.h"

enum class State {Invalid, Initial};

void perish();
void cleanup();
time_t getTime();
