#pragma once

#include "gtp_lz.h"

#include <sstream>

class GtpLzStub : public GtpLz
{
public:

	GtpLzStub(const std::string& e = std::string());
	std::string getLine() override;
	std::string getLine(const std::string& s) override;
	std::string getResult() override;
	void send(const std::string& gtpCommand) override;
	std::string str() const;

private:

	std::istringstream ess;
	std::ostringstream oss;
};
