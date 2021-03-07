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