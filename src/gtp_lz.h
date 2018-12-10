#pragma once

#include <string>

struct GtpLz
{
	void skipNextBlankLine();
	virtual std::string getLine() = 0;
	virtual std::string getLine(const std::string& s) = 0;
	virtual std::string getResult() = 0;
	virtual void send(const std::string& gtpCommand) = 0;
};
