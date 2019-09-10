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

#include "../../settings/Settings.h"

static void KVK_checkVKRresult(VkResult err) {
	if (err == 0)
		return;
  Settings::Instance()->funcDoLog(Settings::Instance()->string_format("VkResult %d\n", err));
	if (err < 0)
		abort();
}

static VKAPI_ATTR VkBool32 VKAPI_CALL KVK_Vulkan_DebugReport(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData) {
  (void)flags; (void)object; (void)location; (void)messageCode; (void)pUserData; (void)pLayerPrefix; // Unused arguments
  fprintf(stderr, "[KuplungVK] ObjectType: %i\nMessage: %s\n\n", objectType, pMessage);
  return VK_FALSE;
}

#endif /* VulkanIncludes_h */
