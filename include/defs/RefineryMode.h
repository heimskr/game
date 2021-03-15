#pragma once

#include <array>

enum class RefineryMode {Clarify, Congeal, Polymerize};
extern std::array<RefineryMode, 3> refineryModes;
const char * stringify(RefineryMode);
