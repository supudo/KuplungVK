#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <type_traits>
#include <typeinfo>
#include <memory>
#include <functional>
#include "SettingsStructs.h"
#include "ConfigUtils.hpp"

class Settings {
public:
	static Settings* Instance();
	void initSettings(const std::string& iniFolder);
	void saveSettings();
	std::string appFolder();
	void setLogFunc(const std::function<void(std::string)>& doLog);
  const std::string prettyBbytes(uint64_t bytes) const;
  void logTimings(const std::string& file, const std::string& method);

	std::function<void(std::string)> funcDoLog;

  std::string UIFontFile, newLineDelimiter, appVersion;

  int SDL_Window_Flags;
	int SDL_Window_Width, SDL_Window_Height;
	int frameLog_Width, frameLog_Height;

  int SelectedGPU;
  int Consumption_Interval_CPU, Consumption_Interval_Memory;
  std::vector<std::string> AvailableGPUs;
  bool VulkanDebugMode;

  int UIFontFileIndex;
  float UIFontSize;

  bool logDebugInfo;
  bool showFrameRenderTime;

	std::string ApplicationConfigurationFolder, currentFolder;
	int Setting_CurrentDriveIndex, Setting_SelectedDriveIndex;
	std::vector<std::string> hddDriveList;

	Color guiClearColor;

#ifdef _WIN32
  template<typename ... Args>
  std::string string_format(const std::string& format, Args ... args) {
    size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1;
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format.c_str(), args ...);
    return std::string(buf.get(), buf.get() + size - 1);
  }
#else
  std::string string_format(const std::string& fmt_str, ...);
#endif

private:
	Settings() {}
	Settings(Settings const&) {}
	Settings& operator=(Settings const&);
	static Settings* m_pInstance;
  std::unique_ptr<ConfigUtils> cfgUtils;
};
