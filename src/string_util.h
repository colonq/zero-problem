#pragma once

#include <string>

#ifdef _WIN32
using Char = wchar_t;
using String = std::wstring;
#else
using Char = char;
using String = std::string;
#endif

std::string ToUtf8String(const String& str);
String ToString(const std::string& utf8str);
void Trim(std::string& str, char c);
void TrimBlankSpaces(std::string& str);
std::pair<std::string, std::string> SplitByLastSpace(const std::string& str);
std::string Escape(const std::string& str, char closeBracket);
std::string Unescape(const std::string& str);
std::string getConfigDir();

class ConvertToUtf8
{
public:
	ConvertToUtf8();
	ConvertToUtf8(const std::string& charset);
	~ConvertToUtf8();

	bool isOk();
	std::string operator()(const std::string& str);
 
private:
	void* conv;
};
