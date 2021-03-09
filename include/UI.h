#pragma once

#include "ImGui.h"
#include <unordered_map>

namespace UI {
	extern std::unordered_map<const char *, ImFont *> fontMap;

	ImFont * addFont(void *ttf_data, int ttf_size, float size_pixels, const ImFontConfig *config, const ImWchar *glyph_ranges);
	ImFont * getFont(const char *);
}
