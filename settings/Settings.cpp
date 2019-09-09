#include "Settings.h"
#include <memory>
#include <stdarg.h>

#include <string>
#include <vector>

#ifdef _WIN32
#undef main
#include <boost/filesystem.hpp>
#include <iostream>
#include <Windows.h>
#elif __APPLE__
#include "CoreFoundation/CoreFoundation.h"
#else
#include <boost/filesystem.hpp>
#include <iostream>
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_vulkan.h>

Settings* Settings::m_pInstance = NULL;

Settings* Settings::Instance() {
	if (!m_pInstance)
		m_pInstance = new Settings;
	return m_pInstance;
}

void Settings::initSettings(const std::string& iniFolder) {
  m_pInstance->cfgUtils = std::make_unique<ConfigUtils>();
  m_pInstance->cfgUtils->init(iniFolder);

  m_pInstance->appVersion = m_pInstance->cfgUtils->readString("appVersion");

  m_pInstance->guiClearColor = {70.0f / 255.0f, 70.0f / 255.0f, 70.0f / 255.0f, 255.0f / 255.0f};

  m_pInstance->SelectedGPU = m_pInstance->cfgUtils->readInt("SelectedGPU");

  m_pInstance->SDL_Window_Flags = SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
  m_pInstance->SDL_Window_Width = m_pInstance->cfgUtils->readInt("SDL_Window_Width");
  m_pInstance->SDL_Window_Height = m_pInstance->cfgUtils->readInt("SDL_Window_Height");

  printf("--------- %i - %i", m_pInstance->SDL_Window_Width, m_pInstance->SDL_Window_Height);

  m_pInstance->frameLog_Width = m_pInstance->cfgUtils->readInt("frameLog_Width");
  m_pInstance->frameLog_Height = m_pInstance->cfgUtils->readInt("frameLog_Height");

  m_pInstance->UIFontFileIndex = m_pInstance->cfgUtils->readInt("UIFontFileIndex");
  m_pInstance->UIFontSize = m_pInstance->cfgUtils->readFloat("UIFontSize");
  m_pInstance->UIFontFile = m_pInstance->cfgUtils->readString("UIFontFile");

  m_pInstance->logDebugInfo = m_pInstance->cfgUtils->readBool("logDebugInfo");
  m_pInstance->showFrameRenderTime = m_pInstance->cfgUtils->readBool("showFrameRenderTime");
  m_pInstance->Consumption_Interval_CPU = m_pInstance->cfgUtils->readInt("Consumption_Interval_CPU");
  m_pInstance->Consumption_Interval_Memory = m_pInstance->cfgUtils->readInt("Consumption_Interval_Memory");

#ifdef _WIN32
	m_pInstance->Setting_CurrentDriveIndex = 0;
	m_pInstance->Setting_SelectedDriveIndex = 0;
	m_pInstance->hddDriveList.empty();
	DWORD drives = ::GetLogicalDrives();
	if (drives) {
		char drive[] = "?";
		int dc = 0;
		for (int i = 0; i < 26; i++) {
			if (drives & (1 << i)) {
				drive[0] = 'A' + i;
				m_pInstance->hddDriveList.push_back(drive);
				if (&Settings::Instance()->currentFolder[0] == drive) {
					m_pInstance->Setting_CurrentDriveIndex = dc;
					m_pInstance->Setting_SelectedDriveIndex = dc;
				}
			}
			dc += 1;
		}
	}
#endif
	m_pInstance->ApplicationConfigurationFolder = iniFolder;

#ifdef _WIN32
  m_pInstance->newLineDelimiter = "\r\n";
#elif defined macintosh // OS 9
  m_pInstance->newLineDelimiter = "\r";
#else
  m_pInstance->newLineDelimiter = "\n";
#endif
}

std::string Settings::appFolder() {
#ifdef __APPLE__
  CFBundleRef mainBundle = CFBundleGetMainBundle();
  CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
  char folder[PATH_MAX];
  if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8*)folder, PATH_MAX))
    printf("Can't open bundle folder!\n");
  CFRelease(resourcesURL);
  return std::string(folder);
#elif _WIN32
  return boost::filesystem::current_path().string() + "/resources";
#else
  return boost::filesystem::current_path().string() + "/resources";
#endif
}

#ifdef __APPLE__
std::string Settings::string_format(const std::string& fmt_str, ...) {
  int n = static_cast<int>(fmt_str.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
  std::unique_ptr<char[]> formatted;
  va_list ap;
  while (1) {
    formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
    strcpy(&formatted[0], fmt_str.c_str());
    va_start(ap, fmt_str);
    int final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
    va_end(ap);
    if (final_n < 0 || final_n >= n)
      n += abs(final_n - n + 1);
    else
      break;
  }
  return std::string(formatted.get());
}
#endif

void Settings::saveSettings() {
  this->cfgUtils->writeString("appVersion", this->appVersion);
  this->cfgUtils->writeString("currentFolder", this->currentFolder);

  this->cfgUtils->writeBool("logDebugInfo", this->logDebugInfo);
  this->cfgUtils->writeBool("showFrameRenderTime", this->showFrameRenderTime);

  this->cfgUtils->writeInt("SelectedGPU", this->SelectedGPU);

  this->cfgUtils->writeInt("frameLog_Width", this->frameLog_Width);
  this->cfgUtils->writeInt("frameLog_Height", this->frameLog_Height);

  this->cfgUtils->writeInt("SDL_Window_Width", this->SDL_Window_Width);
  this->cfgUtils->writeInt("SDL_Window_Height", this->SDL_Window_Height);

  this->cfgUtils->writeInt("UIFontFileIndex", this->UIFontFileIndex);
  this->cfgUtils->writeFloat("UIFontSize", this->UIFontSize);
  this->cfgUtils->writeString("UIFontFile", this->UIFontFile);

  this->cfgUtils->writeInt("Consumption_Interval_CPU", this->Consumption_Interval_CPU);
  this->cfgUtils->writeInt("Consumption_Interval_Memory", this->Consumption_Interval_Memory);

  this->cfgUtils->saveSettings();
}

void Settings::setLogFunc(const std::function<void(std::string)>& doLog) {
	this->funcDoLog = doLog;
}

std::string Settings::prettyBbytes(uint64_t bytes) {
  const char* suffixes[7];
  suffixes[0] = "B";
  suffixes[1] = "KB";
  suffixes[2] = "MB";
  suffixes[3] = "GB";
  suffixes[4] = "TB";
  suffixes[5] = "PB";
  suffixes[6] = "EB";
  int s = 0;
  double count = bytes;
  while (count >= 1024 && s < 7) {
    s++;
    count /= 1024;
  }
  if (count - floor(count) == 0.0)
    return Settings::Instance()->string_format("%d %s", (int)count, suffixes[s]);
  else
    return Settings::Instance()->string_format("%.1f %s", count, suffixes[s]);
}
