#pragma once

#include <vector>

struct Range
{
	int start;
	int last;
};

std::vector<Range> makeRangeList(int start, int last, int count);

std::vector<int> findKeyPositions(int start, int last, int count, const std::vector<double>& priorities);
