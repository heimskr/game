#pragma once

#include <array>

enum class RefineryMode {Clarify, Polymerize};
extern std::array<RefineryMode, 2> refineryModes;
const char * stringify(RefineryMode);
