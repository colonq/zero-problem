#include "log_file.h"
#include "gtp_lz.h"

void GtpLz::skipNextBlankLine()
{
	std::string line;
	for (;;) {
		line = getLine();
		if (line.empty() || line.front() == '\r') {
			return;
		}
	}
}
