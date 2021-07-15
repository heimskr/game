#pragma once

#include <string>
#include "platform.h"

#ifdef __SWITCH__
extern FsFileSystem fs;
#endif

namespace FS {
	void init();
	bool fileExists(const char *);
	bool dirExists(const char *);
	std::string readFile(const char *);
	void writeFile(const char *, const std::string &);
}
