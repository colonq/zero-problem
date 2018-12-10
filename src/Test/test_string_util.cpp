#include <boost/test/unit_test.hpp>

#include "../string_util.h"
#include <string>
#include <array>
#include <wx/wx.h>

static bool CheckTrim(const char* s1, char c, const char* s2)
{
	std::string str1(s1);
	std::string str2(s2);
	Trim(str1, c);
	return str1 == str2;
}

// #define TEST_SJIS

BOOST_AUTO_TEST_CASE(string_util_Test)
{
	BOOST_CHECK(ToUtf8String(ToString("")) == "");
#ifdef TEST_SJIS
	BOOST_CHECK(ToUtf8String(L"文字列") == "\xE6\x96\x87\xE5\xAD\x97\xE5\x88\x97");
	BOOST_CHECK(ToString("\xE6\x96\x87\xE5\xAD\x97\xE5\x88\x97") == L"文字列");
#endif
	BOOST_CHECK(CheckTrim("", '\r', ""));
	BOOST_CHECK(CheckTrim("abc", '\r', "abc"));
	BOOST_CHECK(CheckTrim("abc\r", '\r', "abc"));
	BOOST_CHECK(CheckTrim("abc\r\r", '\r', "abc\r"));
}

static const char* const multigoCSs[] = {
	/*"ASMO-708",*/ /*"DOS-720",*/ "iso-8859-6", "windows-1256", "iso-8859-4",
	"windows-1257", /*"ibm852",*/ "iso-8859-2", "windows-1250", "gb2312",
	/*"hz-gb-2312",*/ "big5", "cp866", "iso-8859-5", "koi8-r",
	"koi8-u", "windows-1251", "iso-8859-7", "windows-1253", /*"DOS-862",*/
	"iso-8859-8-i", "iso-8859-8", "windows-1255", "iso-2022-jp",
	"shift_jis", /*"ks_c_5601-1987",*/ "windows-874", "iso-8859-9", "windows-1254",
	"utf-8", "windows-1258", "iso-8859-1", "Windows-1252"
};

BOOST_AUTO_TEST_CASE(ConvertToUtf8_Test)
{
	for (auto p : multigoCSs) {
		ConvertToUtf8 conv(p);
		BOOST_CHECK_MESSAGE(conv.isOk(), p);
	}
#ifdef TEST_SJIS
	ConvertToUtf8 convSjis("SJIS");
	BOOST_CHECK_EQUAL(convSjis("日本語"), u8"日本語");
#endif
}
