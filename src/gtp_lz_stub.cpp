#include "gtp_lz_stub.h"
#include "log_file.h"

GtpLzStub::GtpLzStub(const std::string& e)
	: ess(e)
{
}

std::string GtpLzStub::getLine()
{
	std::string line;
	if (ess && std::getline(ess, line)) {
		Log(line);
		return line;
	}
	return std::string();
}

std::string GtpLzStub::getLine(const std::string& s)
{
	std::string line;
	while (ess) {
		line = getLine();		
		if (s.empty()) {
			return line;
		}
		if (line.substr(0U, s.length()) == s) {
			return line;
		}
	}
	return std::string();
}

std::string GtpLzStub::getResult()
{
	Log("=\n");
	return std::string("=\n");
}

void GtpLzStub::send(const std::string& gtpCommand)
{
	oss << gtpCommand << std::endl;
}

std::string GtpLzStub::str() const
{
	return oss.str();
}
