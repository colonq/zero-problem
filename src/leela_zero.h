#pragma once

#include "gtp_lz.h"
#include "string_util.h"

#include <iostream>
#include <sstream>
#include <atomic>
#include <boost/process.hpp>


class LeelaZero : public GtpLz
{
public:

	LeelaZero(const String& lz, const String& networkWeights, int nThreads, std::atomic<bool>& ss);
	~LeelaZero();

	std::string getLine() override;
	std::string getLine(const std::string& s) override;
	std::string getResult() override;
	void send(const std::string& gtpCommand) override;

private:

	boost::process::opstream ops;
	boost::process::ipstream ips;
	boost::process::ipstream eps;
	boost::process::child* child;
	std::atomic<bool>& shouldStop;
};
