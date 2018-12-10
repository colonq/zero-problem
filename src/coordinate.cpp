#include "coordinate.h"

bool Coordinate::isPass() const
{
	return column < 1 || row < 1;
}

const Coordinate& Coordinate::setPass()
{
	column = 0;
	row = 0;
	return *this;
}

const Coordinate& Coordinate::setColumnAndRow(int c, int r, int boardSize)
{
	if (c >= 1 && c <= boardSize && r >= 1 && r <= boardSize) {
		column = c;
		row = r;
	} else {
		column = 0;
		row = 0;
	}
	return *this;
}

const Coordinate& Coordinate::setFromSgfStr(const std::string& word, int boardSize)
{
	if (word.length() == 2U) {
		int c = (word[0] - 'a') + 1;
		int r = (word[1] - 'a') + 1;
		setColumnAndRow(c, r, boardSize);
	} else {
		setPass();
	}
	return *this;
}

const Coordinate& Coordinate::setFromStr(const std::string& word, int boardSize)
{
	if (word.length() >= 2U) {
		int c = (word[0] - 'A') + 1;
		if (word[0] == 'I') {
			setPass();
			return *this;
		} else if (word[0] > 'I') {
			--c;
		}
		int r = word[1] - '0';
		if (r < 1 || r > 9) {
			setPass();
			return *this;
		}
		if (word.length() >= 3U) {
			int r2 = word[2] - '0';
			if (r2 < 0 || r2 > 9) {
				setPass();
				return *this;
			}
			r = r * 10 + r2;
		}
		r = (boardSize + 1) - r;
		setColumnAndRow(c, r, boardSize);
	} else {
		setPass();
	}
	return *this;
}

std::array<Coordinate, 4U> Coordinate::getAdjacent(int boardSize) const
{
	std::array<Coordinate, 4U> c4;
	c4[0].setColumnAndRow(column, row - 1, boardSize);
	c4[1].setColumnAndRow(column - 1, row, boardSize);
	c4[2].setColumnAndRow(column + 1, row, boardSize);
	c4[3].setColumnAndRow(column, row + 1, boardSize);
	return c4;
}

std::string Coordinate::sgfStr() const
{
	if (isPass()) return std::string();

	char coord[3];
	coord[0] = static_cast<char>((column + 'a') - 1);
	coord[1] = static_cast<char>((row + 'a') - 1);
	coord[2] = '\0';
	return std::string(coord);
}

std::string Coordinate::str(int boardSize) const
{
	if (isPass()) return std::string("pass");

	char coord[4];
	coord[0] = static_cast<char>((column + 'A') - 1);
	if (coord[0] >= 'I') {
		++coord[0];
	}
	int n = (boardSize + 1) - row;
	if (n >= 10) {
		coord[1] = '1';
		coord[2] = static_cast<char>((n % 10) + '0');
		coord[3] = '\0';
	} else {
		coord[1] = static_cast<char>((n % 10) + '0');
		coord[2] = '\0';
	}
	return std::string(coord);
}

bool Coordinate::operator ==(const Coordinate& coord) const
{
	return column == coord.column && row == coord.row;
}

bool Coordinate::operator !=(const Coordinate& coord) const
{
	return ! (*this == coord);
}
