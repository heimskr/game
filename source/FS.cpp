#include "FS.h"
#include "main.h"

FsFileSystem fs;

namespace FS {
	void init() {
		FsFileSystem *fsptr = fsdevGetDeviceFileSystem("sdmc");
		if (fsptr == nullptr) {
			print("Unable to initialize filesystem.\n");
			perish();
		} else
			fs = *fsptr;
	}

	bool fileExists(const char *path) {
		FsFile file;
		if (R_SUCCEEDED(fsFsOpenFile(&fs, path, FsOpenMode_Read, &file))) {
			fsFileClose(&file);
			return true;
		}

		return false;
	}

	bool dirExists(const char *path) {
		FsDir dir;
		if (R_SUCCEEDED(fsFsOpenDirectory(&fs, path, FsDirOpenMode_ReadDirs, &dir))) {
			fsDirClose(&dir);
			return true;
		}

		return false;
	}

	std::string readFile(const char *path) {
		if (!FS::fileExists(path))
			throw std::runtime_error("File doesn't exist");

		Result result = 0;
		FsFile file;
		if (R_FAILED(result = fsFsOpenFile(&fs, path, FsOpenMode_Read, &file)))
			throw std::runtime_error("fsFsOpenFile failed: 0x" + hex(result));

		s64 filesize;
		if (R_FAILED(result = fsFileGetSize(&file, &filesize))) {
			fsFileClose(&file);
			throw std::runtime_error("fsFileGetSize failed: 0x" + hex(result));
		}

		if (filesize == 0)
			return {};

		char *text = new char[filesize + 1];
		u64 bytes_read;
		if (R_FAILED(result = fsFileRead(&file, 0, text, filesize, 0, &bytes_read))) {
			delete[] text;
			fsFileClose(&file);
			throw std::runtime_error("fsFileRead failed: 0x" + hex(result));
		}

		fsFileClose(&file);
		std::string out = text;
		delete[] text;
		return out;
	}

	void writeFile(const char *path, const std::string &str) {
		Result result = 0;
		if (!FS::fileExists(path)) {
			if (R_FAILED(result = fsFsCreateFile(&fs, path, 0, 0)))
				throw std::runtime_error("fsFsCreateFile failed: 0x" + hex(result));
		}

		FsFile file;
		if (R_FAILED(result = fsFsOpenFile(&fs, path, FsOpenMode_Write, &file)))
			throw std::runtime_error("fsFsOpenFile failed: 0x" + hex(result));

		if (R_FAILED(result = fsFileSetSize(&file, str.size()))) {
			fsFileClose(&file);
			throw std::runtime_error("Couldn't resize file: 0x" + hex(result));
		}

		if (R_FAILED(result = fsFileWrite(&file, 0, str.c_str(), str.size(), 0))) {
			fsFileClose(&file);
			throw std::runtime_error("fsFileWrite failed: 0x" + hex(result));
		}

		fsFileClose(&file);
	}
}
