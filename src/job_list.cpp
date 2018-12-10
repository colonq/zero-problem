#include "job_list.h"

#include <utility>
#include <algorithm>

void JobList::clear()
{
	jobs.clear();

	setExpectedEndTime(std::chrono::steady_clock::now());
}

int JobList::size() const
{
	return static_cast<int>(jobs.size());
}

bool JobList::include(const String& filename) const
{
	for (const auto& j : jobs) {
		if (j.name == filename) {
			return true;
		}
	}
	return false;
}

int JobList::calculateRemainingTime(const Setting& setting, double secondPerVisit) const
{
	double time = 0.0;
	const int q1 = setting.getVirtualNumberOfVisits1();
	const int q2 = setting.getVirtualNumberOfVisits2();
	for (const auto& j : jobs) {
		if (j.isUnnecessary()) {
			continue;
		}

		int n = 0;
		// j.positionの初期値がj.startの一つ手前を指しているとは限らない
		const int position = std::max(j.position, j.start - 1);

		// 範囲の一つ先の局面まで評価しているので+1する必要がある
		if (position < j.last + 1) {
			n += (j.last + 1 - position) * q1;
		}

		// 問題ごとに最大で二回の探索が必要
		if (j.doubleCountOfProblems < j.numberOfProblems * 2) {
			n += (j.numberOfProblems * 2 - j.doubleCountOfProblems) * q2;
		}
		time += static_cast<double>(n) * secondPerVisit;
	}
	return static_cast<int>(time);
}

std::chrono::steady_clock::time_point JobList::getExpectedEndTime() const
{
	std::chrono::steady_clock::time_point epoch;
	return epoch + std::chrono::nanoseconds(expectedEndTime.load());
}

void JobList::setExpectedEndTime(const std::chrono::steady_clock::time_point& e)
{
	// std::atomic<std::chrono::steady_clock::time_point>はダメなようなので
	// 時間はエポックからのナノセカンドで表す
	const auto t = std::chrono::duration_cast<std::chrono::nanoseconds>(e.time_since_epoch()).count();
	expectedEndTime.store(static_cast<long long int>(t));
}

void JobList::setExpectedEndTime(const Setting& setting, double secondPerVisit)
{
	const auto s = std::chrono::seconds(calculateRemainingTime(setting, secondPerVisit));
	const auto t = std::chrono::steady_clock::now() + s;
	setExpectedEndTime(t);
}

void JobList::appendJob(const Job& job)
{
	jobs.push_back(job);
}

void JobList::deleteJob()
{
	jobs.pop_back();
}

bool JobList::deleteJob(int index)
{
	if (index < 0 || index >= static_cast<int>(jobs.size())) {
		return false;
	}
	jobs.erase(jobs.begin() + index);
	return true;
}

void JobList::applySetting(const Setting& setting)
{
	for (auto&& j : jobs) {
		j.setPlayer(setting);
		j.setNumberOfProblems(setting);
	}
}

void JobList::clearToRegenerate()
{
	for (auto&& j : jobs) {
		j.doubleCountOfProblems = 0;
		j.position = 0;
		j.updated = false;
	}
}
