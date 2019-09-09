#pragma once

#include "../utilities/imgui/imgui.h"
#include "../utilities/imgui/imgui_impl_sdl.h"
#include "../utilities/imgui/imgui_impl_vulkan.h"
#ifdef _WIN32
#include <tchar.h>
#endif
#include <memory>
#include <string>
#include <functional>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.hpp>

#include "components/Log.h"
#include "dialogs/DialogOptions.hpp"

class UIManager {
public:
  UIManager();
  ~UIManager();
	void init(SDL_Window* sdlWindow, ImGui_ImplVulkan_InitInfo init_info, ImGui_ImplVulkanH_Window* windowVulkan,
    const std::function<void()>& funcQuitApp,
    const std::function<void()>& funcNewScene);
	void renderStart();
	void renderEnd(VkCommandBuffer command_buffer);
  void renderUI(SDL_Window* sdlWindow, ImVec4 vkClearColor);
  bool processEvent(SDL_Event *event);

	void doLog(const std::string& message);

  void setSceneSelectedModelObject(int sceneSelectedModelObject);

  bool needsFontChange;

private:
  std::function<void()> funcQuitApp;
  std::function<void()> funcNewScene;

	ImVec4 guiClearColor;

	std::unique_ptr<Log> componentLog;
  std::unique_ptr<DialogOptions> dialogOptions;
  std::unique_ptr<DialogStyle> dialogStyle;

  void renderDialogAppMetrics();
  void renderDialogAboutImGui();
  void renderDialogAboutKuplungVK();
  void renderDialogOptions(ImGuiStyle* ref);
  void renderDialogLog();

  bool showDialogStyle;
  bool showSceneStats;
  bool showOptions;
  bool showAppMetrics;
  bool showAboutImgui;
  bool showAboutKuplungVK;
  bool showDemoWindow;
};

