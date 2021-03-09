#include <stdexcept>
#include "Direction.h"

const char * toString(Direction direction) {
	switch (direction) {
		case Direction::North: return "North";
		case Direction::East:  return "East";
		case Direction::South: return "South";
		case Direction::West:  return "West";
		default: return "???";
	}
}

std::pair<s64, s64> getOffset(Direction direction) {
	switch (direction) {
		case Direction::North: return { 0, -1};
		case Direction::East:  return { 1,  0};
		case Direction::South: return { 0,  1};
		case Direction::West:  return {-1,  0};
		default: throw std::invalid_argument("Invalid Direction");
	}
}
