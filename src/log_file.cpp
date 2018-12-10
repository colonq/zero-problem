#include "log_file.h"

#include <ctime>
#include <iomanip>

Logfile::Logfile() : logfile()
{
}

Logfile::~Logfile()
{
	close();
}

void Logfile::open(const char* filename)
{
	logfile.open(filename);
}

void Logfile::close()
{
	logfile.flush();
	logfile.close();
}

bool Logfile::isOpen() const
{
	return logfile.is_open();
}

void Logfile::operator() (const std::string& utf8line)
{
	if (logfile.is_open()) {
		if (utf8line.length() >= 1U && utf8line.back() == '\r') {
			logfile << utf8line.substr(0U, utf8line.length() - 1U) << std::endl;
		} else {
			logfile << utf8line << std::endl;
		}
	}
}

void Logfile::time()
{
	const std::time_t now = std::time(nullptr);
	logfile << std::put_time(std::localtime(&now), "<Time>\n%H:%M:%S\n") << std::endl;
}

struct Logfile Log;
