#include <stdexcept>

#include "FS.h"
#include "Util.h"

#ifdef __SWITCH__
FsFileSystem fs;
namespace FS {
#include "FS.switch.inc"
}
#else
#include <fstream>
#include <sys/stat.h>

namespace FS {
	void init() {}

	bool fileExists(const char *path) {
		struct stat buffer;
		return stat(path, &buffer) == 0 && (buffer.st_mode & S_IFDIR) == 0;
	}

	bool dirExists(const char *path) {
		struct stat buffer;
		return stat(path, &buffer) == 0 && (buffer.st_mode & S_IFDIR) == S_IFDIR;
	}

	std::string readFile(const char *path) {
		std::ifstream stream(path);
		stream.seekg(0, std::ios::end);
		const size_t size = stream.tellg();
		std::string buffer(size, ' ');
		stream.seekg(0);
		stream.read(&buffer[0], size);
		stream.close();
		return buffer;
	}

	void writeFile(const char *path, const std::string &text) {
		std::ofstream stream(path);
		stream << text;
		stream.close();
	}
}
#endif
