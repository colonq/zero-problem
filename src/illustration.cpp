#include "illustration.h"
#include "log_file.h"

#include <sstream>
#include <iomanip>

void Illustration::clear()
{
	board.clear();
	position = 0;
	referencePosition = 0;
	for (int i = 0; i < 2; ++i) {
		referenceBoards[i].clear();
		winrate[i] = 0.0;
		moves[i].clear();
	}
}

std::string Illustration::json(const std::string& id)
{
	std::ostringstream o;
	const int boardSize = board.size;
	if (boardSize == 0) {
		return std::string();
	}

	const auto& lastMove = board.lastMove;
	const std::string lastMoveCoord = lastMove.coord.isPass() ? "--" : lastMove.coord.sgfStr();
	const char nextMoveColor = lastMove.color == Color::BLACK ? 'W' : 'B';

	o << "\t\t{\n";
	o << "\t\t\tid: '" << id << "',\n";
	o << "\t\t\tsz: " << boardSize << ",\n";
	o << "\t\t\tps: " << (position + 1) << ",\n";
	o << "\t\t\twr: [ " << std::fixed << std::setprecision(2) << winrate[0] << ", ";
	o << std::fixed << std::setprecision(2) << winrate[1] << " ],\n";
	o << "\t\t\tlm: '" << lastMoveCoord << "',\n";
	o << "\t\t\tcl: '" << nextMoveColor << "',\n";

	o << "\t\t\tst: '";
	std::vector<int> v;
	const auto& stones = board.stones;
	for (int row = 1; row <= boardSize; ++row) {
		for (int column = 1; column <= boardSize; ++column) {
			Color c = stones[row - 1][column - 1];
			if (c == Color::BLACK) {
				v.push_back(1);
			} else if (c == Color::WHITE) {
				v.push_back(2);
			} else {
				v.push_back(0);
			}
		}
	}
	while (v.size() % 3U) {
		v.push_back(0);
	}
	const int size = static_cast<int>(v.size());
	for (int i = 0; i < size; i += 3) {
		int a = 0x40 + v[i + 2] * 9 + v[i + 1] * 3 + v[i];
		o << static_cast<char>(a);
	}
	o << "',\n";

	o << "\t\t\tva: [ '";
	for (const auto& m : moves[0]) {
		const auto s = m.coord.sgfStr();
		if (s == "") {
			o << "--";
		} else {
			o << s;
		}
	}
	o << "', '";
	for (const auto& m : moves[1]) {
		const auto s = m.coord.sgfStr();
		if (s == "") {
			o << "--";
		} else {
			o << s;
		}
	}
	o << "' ]\n";

	o << "\t\t}";
	return o.str();
}
