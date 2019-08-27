#include "CPPUtilities.h"

#include <locale>
#include <codecvt>
#include <string>

std::wstring CPPUtilities::convert_to_wstring(const std::string& str) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.from_bytes(str);
}