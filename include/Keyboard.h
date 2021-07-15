/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  WerWolv
    Copyright (C) 2019  p-sam

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <functional>
#include <string>

#ifdef __SWITCH__
#include <switch/types.h>
#endif

class Keyboard {
	public:
		static constexpr int MAX_LENGTH = 64;
		static bool openForText(Context &, std::function<void(std::string)> f, const std::string &headerText = "", const std::string &subText = "", int maxStringLength = MAX_LENGTH, const std::string &initialText = "", u32 kbdDisableBitmask = 0);
		static bool openForNumber(Context &, std::function<void(s64)> f, const std::string &headerText = "", const std::string &subText = "", int maxStringLength = MAX_LENGTH, const std::string &initialText = "", const std::string &leftButton = "", const std::string &rightButton = "", u32 kbdDisableBitmask = 0);
		static bool openForDouble(Context &, std::function<void(double)> f, const std::string &headerText = "", const std::string &subText = "", int maxStringLength = MAX_LENGTH, const std::string &initialText = "", const std::string &leftButton = "", u32 kbdDisableBitmask = 0);
		// Use SwkbdKeyDisableBitmask_* for kbdDisableBitmask.
};

