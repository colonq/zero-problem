#pragma once

#include "variation.h"

#include <iostream>

struct VariationList
{
	std::vector<Variation> variations;

	bool create(GtpLz& gtp, Color color, int boardSize);
};
