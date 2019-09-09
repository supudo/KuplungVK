#pragma once

#include <string>

class CPPUtilities {
public:
	static std::wstring convert_to_wstring(const std::string& str);
  static std::string convert_to_string(const std::wstring& wstr);
};

