#include "string_util.h"

#include <cctype>
#include <sstream>
#include <iomanip>
#include <wx/wx.h>
#include <wx/stdpaths.h>

std::string ToUtf8String(const String& str)
{
#ifdef _WIN32
	wxString s(str);
	return s.ToUTF8().data();
#else
	return str;
#endif
}

String ToString(const std::string& utf8str)
{
#ifdef _WIN32
	wxString s = wxString::FromUTF8(utf8str);
	return s.ToStdWstring();
#else
	return utf8str;
#endif
}

void Trim(std::string& str, char c)
{
	if (!str.empty() && str.back() == c) {
		str = str.substr(0U, str.length() - 1U);
	}
}

void TrimBlankSpaces(std::string& str)
{
	size_t n = 0;
	for (auto it = str.cbegin(); it < str.cend(); ++it) {
		if (*it == ' ' || *it == '\t') {
			++n;
		} else {
			break;
		}
	}
	if (n > 0U) {
		str = str.substr(n);
	}
	n = 0;
	for (auto it = str.crbegin(); it < str.crend(); ++it) {
		if (*it == ' ' || *it == '\t') {
			++n;
		} else {
			break;
		}
	}
	if (n > 0U) {
		str = str.substr(0, str.size() - n);
	}
}

// 文字列strの前後の空白を取ってから最後の空白で区切り
// それぞれの文字列の前後の空白を取り除いたものを戻す
std::pair<std::string, std::string> SplitByLastSpace(const std::string& str)
{
	std::string s = str;

	// 空白文字はタブとスペースしか確認しない
	TrimBlankSpaces(s);
	auto n = str.find_last_of(' ');
	if (n == std::string::npos) {
		n = str.find_last_of('\t');
	}

	// 一単語ならば区切る必要はない
	if (n == std::string::npos) {
		std::string s = str;
		TrimBlankSpaces(s);
		return {s, ""};
	}

	std::string s1 = str.substr(0U, n);
	std::string s2 = str.substr(n);
	TrimBlankSpaces(s1);
	TrimBlankSpaces(s2);
	return {s1, s2};
}

std::string Escape(const std::string& str, char closeBracket)
{
	std::ostringstream o;
	for (const auto& c : str) {
		if (c == '\\' || c == closeBracket) {
			o << '\\';
		}
		o << c;
	}
	return o.str();
}

std::string Unescape(const std::string& str)
{
	std::ostringstream o;
	bool isEscaped = false;
	for (const auto& c : str) {
		if (isEscaped) {
			isEscaped = false;
		} else {
			if (c == '\\') {
				isEscaped = true;
				continue;
			}
		}
		o << c;
	}
	return o.str();
}

std::string getConfigDir()
{
#ifdef _WIN32
	return std::string();
#else
	auto cd = wxStandardPaths::Get().GetUserConfigDir();
	return cd.ToUTF8().data();
#endif
}

ConvertToUtf8::ConvertToUtf8()
	: conv(nullptr)
{
}

ConvertToUtf8::ConvertToUtf8(const std::string& charset)
	: conv(static_cast<void*>(new wxCSConv(charset)))
{
}

ConvertToUtf8::~ConvertToUtf8()
{

	// ここでwxCSConvがメモリーリークしているが回避できない
	delete static_cast<wxCSConv*>(conv);
}

bool ConvertToUtf8::isOk()
{
	if (conv == nullptr) {
		return true;
	}
	return static_cast<wxCSConv*>(conv)->IsOk();
}

std::string ConvertToUtf8::operator()(const std::string& str)
{
	auto c = static_cast<wxCSConv*>(conv);
	if (conv == nullptr || !c->IsOk()) {
		return str;
	} else {
		wxString s(str.c_str(), *c);
		return s.ToUTF8().data();
	}
}
