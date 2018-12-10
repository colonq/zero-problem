#pragma once

#include "board.h"

struct Illustration
{
	Board board;
	int position;
	Board referenceBoards[2];
	int referencePosition;
	double winrate[2];
	std::vector<Move> moves[2];

	void clear();
	std::string json(const std::string& id);
};
