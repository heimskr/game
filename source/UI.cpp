#include <switch.h>

#include "UI.h"
#include "Util.h"

namespace UI {
	std::unordered_map<const char *, ImFont *> fontMap;

	ImFont * addFont(void *ttf_data, int ttf_size, float size_pixels, const ImFontConfig *config, const ImWchar *glyph_ranges) {
		ImGuiIO &io = ImGui::GetIO();
		ImFont *font = io.Fonts->AddFontFromMemoryTTF(ttf_data, ttf_size, size_pixels, config, glyph_ranges);
		fontMap.emplace(config->Name, font);
		return font;
	}

	ImFont * getFont(const char *name) {
		if (fontMap.count(name) != 0)
			return fontMap.at(name);
		ImGuiIO &io = ImGui::GetIO();
		for (int i = 0; i < io.Fonts->Fonts.Size; ++i) {
			if (strcmp(io.Fonts->Fonts[i]->GetDebugName(), name) == 0)
				return io.Fonts->Fonts[i];
		}
		return nullptr;
	}
}
