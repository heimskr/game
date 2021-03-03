#pragma once

#include <functional>
#include <string>
#include <sys/types.h>

#include "Util.h"

enum class State {Invalid, Initial, SelectAction, SelectRegion};

struct Action {
	const char *name;
	State state;
	std::function<void()> onSelect;

	Action(const char *name_, State state_):
		name(name_), state(state_), onSelect([] {}) {}

	Action(const char *name_, State state_, std::function<void()> onSelect_):
		name(name_), state(state_), onSelect(std::move(onSelect_)) {}
};

void perish();
void cleanup();
time_t getTime();
std::string randomString(size_t);
