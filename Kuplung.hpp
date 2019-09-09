//
// Kuplung.hpp
// Kuplung
//
//  Created by Sergey Petrov on 11/13/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef Kuplung_hpp
#define Kuplung_hpp

#include "utilities/ivulkan/VulkanIncludes.h"

#include "utilities/imgui/imgui.h"
#include "utilities/imgui/imgui_impl_sdl.h"
#include "utilities/imgui/imgui_impl_vulkan.h"

#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include "utilities/cpp/CPPUtilities.h"
#elif defined(__APPLE__)
#include <CoreFoundation/CoreFoundation.h>
#include <boost/algorithm/string/replace.hpp>
#endif

#include "settings/Settings.h"
#include "ui/UIManager.hpp"
#include "utilities/controls/ControlsManager.hpp"

class Kuplung {
public:
	Kuplung();
  ~Kuplung();
  int run();

private:
  bool init();
  void initFolders();
  void onEvent(SDL_Event* ev);
  void doLog(std::string const& logMessage);

  void selectBestGPU();
  void SetupVulkan(const char** extensions, uint32_t extensions_count);
  void SetupVulkanWindow(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height);
  void CleanupVulkan();
  void CleanupVulkanWindow();
  void FrameRender(ImGui_ImplVulkanH_Window* wd);
  void FramePresent(ImGui_ImplVulkanH_Window* wd);

  void renderScene();

  void guiQuit();

  // Screen dimension constants
  const char *WINDOW_TITLE = "KuplungVK";
  const unsigned int WINDOW_POSITION_X = SDL_WINDOWPOS_CENTERED;
  const unsigned int WINDOW_POSITION_Y = SDL_WINDOWPOS_CENTERED;

  // Vulka & SDL
  ImGui_ImplVulkanH_Window* windowVulkan;
  VkAllocationCallbacks* g_Allocator;
  VkInstance g_Instance;
  VkPhysicalDevice g_PhysicalDevice;
  VkDevice g_Device;
  uint32_t g_QueueFamily;
  VkQueue g_Queue;
  VkDebugReportCallbackEXT g_DebugReport;
  VkPipelineCache g_PipelineCache;
  VkDescriptorPool g_DescriptorPool;

  ImGui_ImplVulkanH_Window g_MainWindowData;
  uint32_t g_MinImageCount;
  bool g_SwapChainRebuild;
  int g_SwapChainResizeWidth, g_SwapChainResizeHeight;
  SDL_Window *sdlWindow = NULL;
  SDL_GLContext glContext;

  // Variables
  bool gameIsRunning = false;

  // Customs
  std::unique_ptr<UIManager> managerUI;
  std::unique_ptr<ControlsManager> managerControls;
};

#endif /* Kuplung_hpp */
