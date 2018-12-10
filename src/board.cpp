#include "board.h"
#include "log_file.h"

#include <bitset>
#include <boost/regex.hpp>

void Board::clear()
{
	stones.clear();
	lastMove.clear();
	blackPrisoners = 0;
	whitePrisoners = 0;
	hash.clear();
	size = 0;
}

void Board::clear(int sz)
{
	clear();
	if (sz == 9 || sz == 13 || sz == 19) {
		size = sz;
		for (int row = 1; row <= sz; ++row) {
			std::vector<Color> line;
			for (int column = 1; column <= sz; ++column) {
				line.push_back(Color::EMPTY);
			}
			stones.push_back(line);
		}
	}
}

void Board::removeStones(const std::vector<Coordinate>& deadStones)
{
	for (const auto& c : deadStones) {
		const auto cy = static_cast<unsigned int>(c.row - 1);
		const auto cx = static_cast<unsigned int>(c.column - 1);
		stones[cy][cx] = Color::EMPTY;
	}
}

void Board::removeDeadStones(const Color& color, const Coordinate& coord)
{
	if (color == Color::EMPTY) {
		return;
	}
	if (stones[coord.row - 1][coord.column - 1] != color) {
		return;
	}

	// まだ調べていない交点
	std::vector<Coordinate> stack;
	stack.push_back(coord);

	// 同じ交点を二度辿らないように記録しておく
	const unsigned int maxSize = 19U * 19U;
	std::bitset<maxSize> visited;

	// 発見した死石の候補
	std::vector<Coordinate> deadStones;
	
	// 調べるべき交点がなくなるまで繰り返す
	while (!stack.empty()) {
		const auto c = stack.back();
		stack.pop_back();
		const auto cy = static_cast<unsigned int>(c.row - 1);
		const auto cx = static_cast<unsigned int>(c.column - 1);
		visited.set(cy * 19U + cx);
		const auto s = stones[cy][cx];
		if (s == Color::EMPTY) {
			// 今調べている交点が空いているのであれば生きている
			return;
		} else if (s == color) {
			// 今調べている交点が自分の石ならば、周囲の４ケ所のうち
			// まだ調べていない交点をこれから調べるべき交点に追加する
			deadStones.push_back(c);
			for (const auto& d : c.getAdjacent(size)) {
				if (!d.isPass()) {
					const auto dy = static_cast<unsigned int>(d.row - 1);
					const auto dx = static_cast<unsigned int>(d.column - 1);
					if (!visited.test(dy * 19U + dx)) {
						stack.push_back(d);
					}
				}
			}
		}
	}

	// ダメを一つも発見できなければ死んでいるのでアゲ石に追加する
	if (color == Color::BLACK) {
		whitePrisoners += static_cast<int>(deadStones.size());
	} else {
		blackPrisoners += static_cast<int>(deadStones.size());
	}

	// 死石を取り除く
	removeStones(deadStones);
}

void Board::play(const Move& move)
{
	// 棋譜が間違っているのはこのソフトの責任ではないので
	// 自殺手の確認やコウの確認はしないで常に合法手として扱う
	lastMove = move;

	// パスならば何もしない
	const auto& c = move.coord;
	if (c.isPass()) {
		return;
	}

	// とりあえずその手の位置が空いていれば石を置いてしまう
	const auto cy = static_cast<unsigned int>(c.row - 1);
	const auto cx = static_cast<unsigned int>(c.column - 1);
	if (stones[cy][cx] != Color::EMPTY) {
		return;
	}
	stones[cy][cx] = move.color;

	// 四方に相手の石があれば死んでいるか確認して死んでいれば取る
	const Color o = OpponentColor(move.color);
	for (const auto& d : c.getAdjacent(size)) {
		if (!d.isPass()) {
			removeDeadStones(o, d);
		}
	}

}

bool Board::readStoneLine(GtpLz& gtp, int& row)
{
	std::vector<Color> stoneLine;
	const std::string line = gtp.getLine();
	const int len = static_cast<int>(line.length());
	if (len < 3 || line.substr(0U, 3U) == "   ") {
		return false;
	}
	for (int i = 2, column = 1; i + 1 < len; i += 2, ++column) {
		char c = line[i + 1];
		if (c == 'X') {
			stoneLine.push_back(Color::BLACK);
			if (line[i] == '(') {
				lastMove.color = Color::BLACK;
				lastMove.coord.column = column;
				lastMove.coord.row = row;
			}
		} else if (c == 'O') {
			stoneLine.push_back(Color::WHITE);
			if (line[i] == '(') {
				lastMove.color = Color::WHITE;
				lastMove.coord.column = column;
				lastMove.coord.row = row;
			}
		} else if (c == '.' || c == '+') {
			stoneLine.push_back(Color::EMPTY);
		} else {
			break;
		}
	}
	++row;
	stones.push_back(std::move(stoneLine));
	return true;
}


// Leela Zero のshowboardコマンドの出力をそのまま読んでいる
static const boost::regex rePrisoners(R"(Prisoners: (\d++))");
static const boost::regex reHash(R"(Hash: (\S++))");
bool Board::create(GtpLz& gtp)
{
	std::string line;
	boost::smatch what;

	// 「Passes:」で始まっている行までの全ての行を読み飛ばす
	line = gtp.getLine("Passes:");
	blackPrisoners = 0;
	if (boost::regex_search(line, what, rePrisoners)) {
		blackPrisoners = std::stoi(what[1].str());
	}

	line = gtp.getLine();
	whitePrisoners = 0;
	if (boost::regex_search(line, what, rePrisoners)) {
		whitePrisoners = std::stoi(what[1].str());
	}

	// スペース三つで始まっている行までの全ての行を読み飛ばす
	gtp.getLine("   ");
	lastMove.coord.setPass();
	int row = 1;
	stones.clear();
	while (readStoneLine(gtp, row)) {}

	gtp.skipNextBlankLine();

	line = gtp.getLine();
	if (boost::regex_search(line, what, reHash)) {
		hash = what[1].str();
	}
	gtp.getLine();

	gtp.skipNextBlankLine();

	// 碁盤表示部分の行数を碁盤のサイズとする
	// 碁盤が正方形でなければエラーとする
	size = static_cast<int>(stones.size());
	for (const auto& s : stones) {
		if (static_cast<int>(s.size()) != size) {
			clear();
			return false;
		}
	}

	return true;
}
