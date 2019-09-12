#include "CPPUtilities.h"

#include <locale>
#include <codecvt>
#include <string>

const std::wstring CPPUtilities::convert_to_wstring(const std::string& str) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.from_bytes(str);
}

const std::string CPPUtilities::convert_to_string(const std::wstring& wstr) {
  using convert_typeX = std::codecvt_utf8<wchar_t>;
  std::wstring_convert<convert_typeX, wchar_t> converterX;
  return converterX.to_bytes(wstr);
}
