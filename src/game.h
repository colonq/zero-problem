#pragma once

#include "move.h"
#include "gtp_lz.h"

#include <string>
#include <vector>

struct Game
{
	int boardSize;
	std::string characterSet;
	std::string blackPlayer;
	std::string blackRank;
	std::string whitePlayer;
	std::string whiteRank;
	std::string place;
	std::string result;
	std::string ruleset;
	double komi;
	int handicap;
	std::string date;
	std::vector<Move> stones;
	std::vector<Move> moves;
	int currentPosition;

	void clear();
	std::vector<std::string> parseSgfToken(const std::string& token) const;
	std::vector<std::vector<std::string>> parseSgf(const std::string& sgf) const;
	bool normalize(const char* defaultCharacterSet);
	bool readSgf(const std::string& sgf, const char* defaultCharacterSet);
	bool setHandicapStones(int n, bool jp = false);
	bool readGib(const std::string& gib, const char* defaultCharacterSet);
	bool readNgf(const std::string& ngf, const char* defaultCharacterSet);
	bool readUgf(const std::string& ugf, const char* defaultCharacterSet);
	std::string sgfStr() const;
	std::string ngfStr() const;

	bool play(GtpLz& gtp);
	bool undo(GtpLz& gtp);
	bool go(int position, GtpLz& gtp);
	bool moveTo(int position, GtpLz& gtp);
};
