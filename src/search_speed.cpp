#include "search_speed.h"

void SearchSpeed::clear()
{
	startTime = std::chrono::steady_clock::now();
	numberOfVisits = 0;
	// secondPerVisitはクリアしない
}

void SearchSpeed::clear(double spp)
{
	startTime = std::chrono::steady_clock::now();
	numberOfVisits = 0;
	secondPerVisit = spp;
}

void SearchSpeed::addNumberOfVisits(int n)
{
	using fsec = std::chrono::duration<double>;
	const auto now = std::chrono::steady_clock::now();
	const auto e = std::chrono::duration_cast<fsec>(now - startTime).count();
	numberOfVisits += n;
	if (numberOfVisits > 0) {
		secondPerVisit = e / static_cast<double>(numberOfVisits);
	}
}

double SearchSpeed::getSecondPerVisit() const
{
	return secondPerVisit;
}
