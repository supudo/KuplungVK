#pragma once

#include "../utilities/imgui/imgui.h"
#include "../utilities/imgui/imgui_impl_sdl.h"
#include "../utilities/imgui/imgui_impl_vulkan.h"
#ifdef _WIN32
#include <tchar.h>
#endif
#include <memory>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.hpp>

#include "components/Log.h"

class UIManager {
public:
	void init(SDL_Window* sdlWindow, ImGui_ImplVulkan_InitInfo init_info, ImGui_ImplVulkanH_Window* wd);
	void renderStart();
	void renderEnd();
  bool processEvent(SDL_Event *event);

	void doLog(const std::string& message);

	bool Show_ImGui_DemoWindow, Show_ImGui_AnotherWindow;

private:
	ImVec4 guiClearColor;

	std::unique_ptr<Log> componentLog;

	void dialogLog();
};
