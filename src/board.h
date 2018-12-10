#pragma once

#include "move.h"
#include "gtp_lz.h"

#include <vector>
#include <iostream>

struct Board
{
	std::vector<std::vector<Color>> stones;
	Move lastMove;
	int blackPrisoners, whitePrisoners;
	std::string hash;
	int size;

	void clear();
	void clear(int sz);
	void removeStones(const std::vector<Coordinate>& deadStones);
	void removeDeadStones(const Color& color, const Coordinate& coord);
	void play(const Move& move);
	bool readStoneLine(GtpLz& gtp, int& row);
	bool create(GtpLz& gtp);
};
