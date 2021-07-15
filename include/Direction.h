#pragma once

#include <utility>
#include "platform.h"

enum class Direction: int {North = 0, East, South, West};
const char * toString(Direction);
std::pair<s64, s64> getOffset(Direction);
