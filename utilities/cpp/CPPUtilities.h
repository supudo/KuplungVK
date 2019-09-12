#pragma once

#include <string>

class CPPUtilities {
public:
	const static std::wstring convert_to_wstring(const std::string& str);
  const static std::string convert_to_string(const std::wstring& wstr);
};

