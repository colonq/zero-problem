#pragma once

#include "string_util.h"

#include <vector>

enum class Player : int
{
	BLACK,
	WHITE,
	BOTH,
	BY_NAME,
	NONE
};

enum class CharacterSet : int
{
	UTF8,
	SHIFT_JIS,
	GB2312,
	EUC_KR
};

struct Setting
{
	String lz;
	String networkWeights;
	int numberOfThreads;
	int numberOfVisits;
	int numberOfProblems;
	int start;
	int last;
	int maximumNumberOfMoves;
	Player player;
	std::vector<String> playerNames;
	CharacterSet dafaultCharacterSet;
	double searchSpeed;
	bool logging;
	int boardSize;

	void setDefaultValues();
	int getNumberOfVisitsInTheFirstStep() const;
	int getVirtualNumberOfVisits1() const;
	int getVirtualNumberOfVisits2() const;
	bool load();
	bool save() const;
};
