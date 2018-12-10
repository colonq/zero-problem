#pragma once

#include "game.h"
#include "leela_zero.h"
#include "variation_list.h"
#include "setting.h"
#include "job_list.h"
#include "search_speed.h"
#include "exit_status.h"

#include <memory>
#include <atomic>
#include <set>

struct Analysis
{
	Game game;
	std::unique_ptr<LeelaZero> gtp;
	std::vector<std::unique_ptr<VariationList>> vll;

	Analysis();
	~Analysis();
	void reset(int numberOfVisits);
	void evaluate(int position);
	double calculatePriority(Color c1, double v1, Color c2, double v2);
	std::vector<double> measurePriorities(Player player);
	std::vector<std::string> makeJson(
		const std::string& id, int& number,
		int maximumNumberOfMoves, const std::vector<int>& problemPositions);
	std::vector<std::string> generateProblems(
		const Setting& setting, Job& job, JobList& jobList,
		SearchSpeed& searchSpeed, std::atomic<bool>& ss,
		const std::string& id, int& number, std::set<std::string>& hashes);
	std::string generateHtml(
		const Setting& setting, JobList& jobList,
		SearchSpeed& searchSpeed, std::atomic<bool>& ss, 
		const std::string& title, const std::string& id, ExitStatus& exitStatus);
};
