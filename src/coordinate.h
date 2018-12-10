#pragma once

#include <string>
#include <array>

struct Coordinate
{
	int column;
	int row;

	bool isPass() const;
	const Coordinate& setPass();
	const Coordinate& setColumnAndRow(int c, int r, int boardSize);
	const Coordinate& setFromSgfStr(const std::string& word, int boardSize);
	const Coordinate& setFromStr(const std::string& word, int boardSize);
	std::array<Coordinate, 4U> getAdjacent(int boardSize) const;
	std::string sgfStr() const;
	std::string str(int boardSize) const;

	bool operator ==(const Coordinate& coord) const;
	bool operator !=(const Coordinate& coord) const;
};
