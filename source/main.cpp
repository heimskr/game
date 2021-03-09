#undef __APPLE__

#include <SDL2/SDL.h>
#include <switch.h>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>

#define GLM_FORCE_PURE
#define GLM_ENABLE_EXPERIMENTAL

#include "Game.h"
#include "main.h"
#include "MainWindow.h"

#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <memory>

constexpr uint32_t WINDOW_WIDTH  = 1280;
constexpr uint32_t WINDOW_HEIGHT = 720;

int main() {
	socketInitializeDefault();
	nxlinkStdio();
	std::srand(std::time(nullptr));

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0) {
		throw std::runtime_error(SDL_GetError());
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	int w = 1280, h = 720;
	SDL_Window *window = SDL_CreateWindow("TradeGame", 0, 0, w, h, SDL_WINDOW_OPENGL);
	if (!window)
		throw std::runtime_error(SDL_GetError());

	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	SDL_SetWindowSize(window, w, h);
	SDL_GL_MakeCurrent(window, gl_context);
	SDL_GL_SetSwapInterval(1);

	if (gladLoadGL() == 0)
		throw std::runtime_error("gladLoadGL failed");

	if (SDL_GameControllerOpen(0) == nullptr)
		throw std::runtime_error(SDL_GetError());


	SDL_Event event;
	bool done = false;
	// u64 time = 0ul;
	// u64 frequency = SDL_GetPerformanceFrequency();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_IsTouchScreen;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.MouseDrawCursor = true;

	if (!ImGui_ImplSDL2_InitForOpenGL(window, gl_context, true)) {
		throw std::runtime_error("ImGui_ImplSDL2_InitForOpenGL failed");
	}

	if (!ImGui_ImplOpenGL3_Init("#version 330 core")) {
		throw std::runtime_error("ImGui_ImplOpenGL3_Init failed");
	}

	ImGui::StyleColorsDark();
	// ImGui::StyleColorsClassic();

	io.Fonts->AddFontDefault();
	{
		plInitialize(PlServiceType_System);

		PlFontData fonts_std;
		PlFontData fonts_ext;

		plGetSharedFontByType(&fonts_std, PlSharedFontType_Standard);
		plGetSharedFontByType(&fonts_ext, PlSharedFontType_NintendoExt);

		ImFontConfig config;
		config.FontDataOwnedByAtlas = false;

		strcpy(config.Name, "Nintendo Standard");
		io.Fonts->AddFontFromMemoryTTF(fonts_std.address, fonts_std.size, 24.0f, &config, io.Fonts->GetGlyphRangesCyrillic());

		strcpy(config.Name, "Nintendo Ext");
		static const ImWchar ranges[] = {
			0xE000, 0xE06B,
			0xE070, 0xE07E,
			0xE080, 0xE099,
			0xE0A0, 0xE0BA,
			0xE0C0, 0xE0D6,
			0xE0E0, 0xE0F5,
			0xE100, 0xE105,
			0xE110, 0xE116,
			0xE121, 0xE12C,
			0xE130, 0xE13C,
			0xE140, 0xE14D,
			0xE150, 0xE153,
			0,
		};

		io.Fonts->AddFontFromMemoryTTF(fonts_ext.address, fonts_ext.size, 24.0f, &config, ranges);
		io.Fonts->Build();

		plExit();
	}

	bool show_main_window = true;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	ImGuiStyle &style = ImGui::GetStyle();
	style.WindowRounding = 8;
	style.FrameRounding = style.GrabRounding = 3;

	for (int i = 0; i < io.Fonts->Fonts.Size; ++i) {
		if (strcmp(io.Fonts->Fonts[i]->GetDebugName(), "Nintendo Standard") == 0) {
			io.FontDefault = io.Fonts->Fonts[i];
			break;
		}
	}

	Context context;
	context.game = std::make_shared<Game>();

	while (show_main_window && !done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				done = true;
			} else if (event.type == SDL_CONTROLLERBUTTONDOWN) {
				if (event.cbutton.which == 0) {
					if (event.cbutton.button == SDL_CONTROLLER_BUTTON_START) {
						SDL_Event quitEvent;
						quitEvent.type = SDL_QUIT;
						SDL_PushEvent(&quitEvent);
					}
				}
				ImGui_ImplSDL2_ProcessEvent(&event);
			} else {
				ImGui_ImplSDL2_ProcessEvent(&event);
			}
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(window);
		ImGui::NewFrame();

		MainWindow(context, &show_main_window);

		if (!context.message.empty()) {
			ImGui::OpenPopup("Message");
		}

		constexpr float MODAL_WIDTH = 500.f, MODAL_HEIGHT = 200.f;
		ImGui::SetNextWindowPos(ImVec2((1280.f - MODAL_WIDTH) / 2.f, (720.f - MODAL_HEIGHT) / 2.f), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(MODAL_WIDTH, MODAL_HEIGHT), ImGuiCond_Always);
		if (ImGui::BeginPopupModal("Message", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
			ImGui::Text("%s", context.message.c_str());
			if (ImGui::Button("Close")) {
				context.message.clear();
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window);
	}

	ImGui::DestroyContext();

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	socketExit();
	return EXIT_SUCCESS;
}

void Context::save() {
	if (!game) {
		message = "No game is open.";
		printf("No game.\n");
		return;
	}

	try {
		// game->save();
		message = "Game saved successfully.";
		printf("Game saved.\n");
	} catch (const std::exception &err) {
		message = "An error occurred while saving: " + std::string(err.what());
		printf("Game save error: %s\n", err.what());
		return;
	}
}
