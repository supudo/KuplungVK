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

Settings* Settings::m_pInstance = NULL;

Settings* Settings::Instance() {
	if (!m_pInstance)
		m_pInstance = new Settings;
	return m_pInstance;
}

void Settings::initSettings(const std::string& iniFolder) {
  m_pInstance->guiClearColor = {70.0f / 255.0f, 70.0f / 255.0f, 70.0f / 255.0f, 255.0f / 255.0f};

	m_pInstance->MainWindow_Width = 1400;
	m_pInstance->MainWindow_Height = 800;
	m_pInstance->frameLog_Width = 300;
	m_pInstance->frameLog_Height = 200;

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

void Settings::setLogFunc(const std::function<void(std::string)>& doLog) {
	this->funcDoLog = doLog;
}
