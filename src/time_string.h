#pragma once

#include "string_util.h"

struct TimeString
{
	String fileName;
	String fileName_M;
	std::string title;
	std::string title_M;
	std::string id;
};

TimeString GetTimeString();
