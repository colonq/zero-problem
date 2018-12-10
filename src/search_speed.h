#pragma once

#include <chrono>

class SearchSpeed
{
public:
	void clear();
	void clear(double spv);
	void addNumberOfVisits(int n);
	double getSecondPerVisit() const;

private:
	std::chrono::steady_clock::time_point startTime;
	int numberOfVisits;
	double secondPerVisit;
};
