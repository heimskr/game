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

#include <cstring>
#include <iostream>
#include <switch.h>

#include "Keyboard.h"
#include "Util.h"

static SwkbdConfig createSwkbdBaseConfig(const std::string &headerText, const std::string &subText, int maxStringLength, const std::string &initialText) {
	SwkbdConfig config;

	swkbdCreate(&config, 0);

	swkbdConfigMakePresetDefault(&config);
	swkbdConfigSetHeaderText(&config, headerText.c_str());
	swkbdConfigSetSubText(&config, subText.c_str());
	swkbdConfigSetStringLenMax(&config, maxStringLength);
	swkbdConfigSetInitialText(&config, initialText.c_str());
	swkbdConfigSetStringLenMaxExt(&config, 1);
	swkbdConfigSetBlurBackground(&config, true);

	return config;
}

bool Keyboard::openForText(std::function<void(std::string)> f, const std::string &headerText, const std::string &subText, int maxStringLength, const std::string &initialText, u32 kbdDisableBitmask) {
	SwkbdConfig config = createSwkbdBaseConfig(headerText, subText, maxStringLength, initialText);

	swkbdConfigSetType(&config, SwkbdType_Normal);
	swkbdConfigSetKeySetDisableBitmask(&config, kbdDisableBitmask);

	char *buffer = new char[maxStringLength + 1];

	if (R_SUCCEEDED(swkbdShow(&config, buffer, maxStringLength)) && strcmp(buffer, "") != 0) {
		f(buffer);
		swkbdClose(&config);
		delete[] buffer;
		return true;
	}

	swkbdClose(&config);
	delete[] buffer;
	return false;
}

bool Keyboard::openForNumber(std::function<void(s64)> f, const std::string &headerText, const std::string &subText, int maxStringLength, const std::string &initialText, const std::string &leftButton, const std::string &rightButton, u32 kbdDisableBitmask) {
	SwkbdConfig config = createSwkbdBaseConfig(headerText, subText, maxStringLength, initialText);

	swkbdConfigSetType(&config, SwkbdType_NumPad);
	swkbdConfigSetLeftOptionalSymbolKey(&config, leftButton.c_str());
	swkbdConfigSetRightOptionalSymbolKey(&config, rightButton.c_str());
	swkbdConfigSetKeySetDisableBitmask(&config, kbdDisableBitmask);

	char *buffer = new char[maxStringLength + 1];

	if (R_SUCCEEDED(swkbdShow(&config, buffer, maxStringLength)) && strcmp(buffer, "") != 0) {
		swkbdClose(&config);
		try {
			f(std::stoll(buffer));
		} catch (const std::invalid_argument &) {
			delete[] buffer;
			return false;
		}

		delete[] buffer;
		return true;
	}

	swkbdClose(&config);
	delete[] buffer;
	return false;
}

bool Keyboard::openForDouble(std::function<void(double)> f, const std::string &headerText, const std::string &subText, int maxStringLength, const std::string &initialText, const std::string &leftButton, u32 kbdDisableBitmask) {
	SwkbdConfig config = createSwkbdBaseConfig(headerText, subText, maxStringLength, initialText);

	swkbdConfigSetType(&config, SwkbdType_NumPad);
	swkbdConfigSetLeftOptionalSymbolKey(&config, leftButton.c_str());
	swkbdConfigSetRightOptionalSymbolKey(&config, ".");
	swkbdConfigSetKeySetDisableBitmask(&config, kbdDisableBitmask);

	char *buffer = new char[maxStringLength + 1];

	if (R_SUCCEEDED(swkbdShow(&config, buffer, maxStringLength)) && strcmp(buffer, "") != 0) {
		swkbdClose(&config);
		try {
			f(std::stod(buffer));
		} catch (const std::invalid_argument &) {
			delete[] buffer;
			return false;
		}

		delete[] buffer;
		return true;
	}

	swkbdClose(&config);
	delete[] buffer;
	return false;
}
