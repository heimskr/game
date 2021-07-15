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

bool Keyboard::openForText(Context &, std::function<void(std::string)> f, const std::string &headerText, const std::string &subText, int maxStringLength, const std::string &initialText, u32 kbdDisableBitmask) {
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

bool Keyboard::openForNumber(Context &, std::function<void(s64)> f, const std::string &headerText, const std::string &subText, int maxStringLength, const std::string &initialText, const std::string &leftButton, const std::string &rightButton, u32 kbdDisableBitmask) {
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

bool Keyboard::openForDouble(Context &, std::function<void(double)> f, const std::string &headerText, const std::string &subText, int maxStringLength, const std::string &initialText, const std::string &leftButton, u32 kbdDisableBitmask) {
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
