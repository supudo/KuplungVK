#ifndef VulkanIncludes_h
#define VulkanIncludes_h

// Enable the WSI extensions
#if defined(__ANDROID__)
#define VK_USE_PLATFORM_ANDROID_KHR
#elif defined(__linux__)
#define VK_USE_PLATFORM_XLIB_KHR
#elif defined(_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#define WIN32_LEAN_AND_MEAN
// Windows Header Files
#include <windows.h>
#include <shlobj.h>
#endif

// Tell SDL not to mess with main()
#define SDL_MAIN_HANDLED

#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.hpp>

#ifdef KVK_VulkanDebug
#define IMGUI_VULKAN_DEBUG_REPORT
#endif

#include "../../settings/Settings.h"

static void KVK_checkVKRresult(VkResult err) {
	if (err == 0)
		return;
  Settings::Instance()->funcDoLog(Settings::Instance()->string_format("VkResult %d\n", err));
	if (err < 0)
		abort();
}

#endif /* VulkanIncludes_h */
