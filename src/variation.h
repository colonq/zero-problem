#pragma once

#include "move.h"
#include "gtp_lz.h"

#include <vector>

struct Variation
{
	Move move;
	int playout;
	double v, n;
	std::vector<Move> pv;

	bool create(GtpLz& gtp, Color color, int boardSize);
};

