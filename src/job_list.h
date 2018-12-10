#pragma once

#include "job.h"

#include <chrono>
#include <atomic>

struct JobList
{
	std::vector<Job> jobs;
	std::atomic<long long int> expectedEndTime;

	void clear();
	int size() const;
	bool include(const String& filename) const;
	int calculateRemainingTime(const Setting& setting, double secondPerVisit) const;
	std::chrono::steady_clock::time_point getExpectedEndTime() const;
	void setExpectedEndTime(const std::chrono::steady_clock::time_point& e);
	void setExpectedEndTime(const Setting& setting, double secondPerVisit);
	void appendJob(const Job& job);
	void deleteJob();
	bool deleteJob(int index);
	void applySetting(const Setting& setting);
	void clearToRegenerate();

};
