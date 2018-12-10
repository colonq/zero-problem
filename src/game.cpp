#include "game.h"
#include "string_util.h"

#include <sstream>
#include <iomanip>
#include <boost/regex.hpp>

void Game::clear()
{
	boardSize = 19;
	characterSet.clear();
	blackPlayer.clear();
	blackRank.clear();
	whitePlayer.clear();
	whiteRank.clear();
	place.clear();
	result.clear();
	ruleset.clear();
	komi = 0.0;
	handicap = 0;
	date.clear();
	stones.clear();
	moves.clear();
	currentPosition = 0;
}

// 構文解析の速度はどうでもよいので簡単のため正規表現を使用する
// 以下、[^\\[\]]が\\.の一文字目にマッチしないところがポイント
static const boost::regex reSgfToken(R"((?:\\.|[^\\[\]])++)");
std::vector<std::string> Game::parseSgfToken(const std::string& token) const
{
	std::vector<std::string> strs;
	boost::sregex_iterator it(token.begin(), token.end(), reSgfToken);
	boost::sregex_iterator itEnd;
	while (it != itEnd) {
		strs.push_back(it->str());
		++it;
	}
	return strs;
}

static const boost::regex reSgf(R"([(;)]|[A-Z]++(?:\[(?:\\.|[^\\\]])*+\])++)");
std::vector<std::vector<std::string>> Game::parseSgf(const std::string& sgf) const
{
	std::vector<std::vector<std::string>> props;
	boost::sregex_iterator it(sgf.begin(), sgf.end(), reSgf);
	boost::sregex_iterator itEnd;
	while (it != itEnd) {
		props.push_back(parseSgfToken(it->str()));
		++it;
	}
	return props;
}

static const boost::regex reNormRank(R"((\d++)(?:([Kk])|([Dd]))([Pp]?+).*+)");
bool Game::normalize(const char* defaultCharacterSet)
{
	// ボードサイズが9/19以外の棋譜は、
	// このアプリケーションでは取り扱わない
	if (boardSize != 9 && boardSize != 19) {
		clear();
		return false;
	}

	// 棋譜の最後のパスは手数に含まれないので削除
	while (!moves.empty() && moves.back().coord.isPass()) {
		moves.pop_back();
	}

	// 棋譜として正しくとも着手がないのは対局とは言えないので、
	// このアプリケーションでは取り扱わない
	if (moves.empty()) {
		clear();
		return false;
	}

	// 連続した黒の手で始まる棋譜は、置き碁として扱う
	int count = 0;
	for (auto m : moves) {
		if (m.color != Color::BLACK || m.coord.isPass()) {
			break;
		}
		++count;
	}
	if (count >= 2 && stones.empty()) {
		handicap = count;
		for (int i = 0; i < count; ++i) {
			stones.push_back(moves[i]);
		}
		moves.erase(moves.begin(), moves.begin() + count);
	}

	// 文字コードをUTF-8にする
	if (characterSet != "UTF-8" && characterSet != "utf-8"
		&& characterSet != "UTF8" && characterSet != "utf8") {
		ConvertToUtf8* conv;
		if (characterSet.empty()) {
			conv = new ConvertToUtf8(defaultCharacterSet);
		} else {
			if (characterSet == "KS_C_5601" || characterSet == "ks_c_5601"
				|| characterSet == "KS_C_5601-1987" || characterSet == "ks_c_5601-1987") {
				conv = new ConvertToUtf8("EUC-KR");
			} else {
				conv = new ConvertToUtf8(characterSet);
			}
		}

		blackPlayer = (*conv)(blackPlayer);
		blackRank   = (*conv)(blackRank);
		whitePlayer = (*conv)(whitePlayer);
		whiteRank   = (*conv)(whiteRank);
		place       = (*conv)(place);
		result      = (*conv)(result);
		ruleset     = (*conv)(ruleset);
		date        = (*conv)(date);

		delete conv;
	}
	characterSet = "utf-8";

	// ランクの表記を小文字のk/d/pに統一
	std::string* ranks[] = {&blackRank, &whiteRank};
	for (int i = 0; i < 2; ++i) {
		if (!(ranks[i]->empty())) {
			boost::smatch m;
			if (boost::regex_match(*ranks[i], m, reNormRank)) {
				std::ostringstream o;
				o << m.str(1);
				if (!m.str(2).empty()) {
					o << 'd';
				} else if (!m.str(4).empty()) {
					o << 'p';
				} else {
					o << 'd';
				}
				*ranks[i] = o.str();
			}
		}
	}

	return true;
}

bool Game::readSgf(const std::string& sgf, const char* defaultCharacterSet)
{
	clear();

	auto props = parseSgf(sgf);
	int moveNumber = -1;
	Move m;
	for (const auto& p : props) {
		const auto& type = p[0];
		int size = static_cast<int>(p.size());
		if (type == ";") {
			++moveNumber;
		} else if (type == ")") {
			// 棋譜が変化図を含んでいても、メインの着手進行しか利用しないので、
			// ')'以降は常に無視してかまわない
			break;
		} else if (type == "B" || type == "W") {
			if (type == "B") {
				m.color = Color::BLACK;
			} else {
				m.color = Color::WHITE;
			}
			if (size == 2) {
				m.coord.setFromSgfStr(p[1], boardSize);
			} else {
				m.coord.setPass();
			}
			moves.push_back(m);
		} else if (moveNumber == 0 && size >= 2) {
			const auto value = Unescape(p[1]);
			if (type == "CA") {
				characterSet = value;
			} else if (type == "SZ") {
				try {
					boardSize = std::stoi(value);
				} catch (...) {
					clear();
					return false;
				}
			} else if (type == "PB") {
				blackPlayer = value;
			} else if (type == "BR") {
				blackRank = value;
			} else if (type == "PW") {
				whitePlayer = value;
			} else if (type == "WR") {
				whiteRank = value;
			} else if (type == "PC") {
				place = value;
			} else if (type == "RE") {
				result = value;
			} else if (type == "RU") {
				ruleset = value;
			} else if (type == "KM") {
				try {
					komi = std::stod(value);
				} catch (...) {
					clear();
					return false;
				}
			} else if (type == "HA") {
				try {
					handicap = std::stoi(value);
				} catch (...) {
					clear();
					return false;
				}
			} else if (type == "DT") {
				date = value;
			} else if (type == "AB" || type == "AW") {
				if (type == "AB") {
					m.color = Color::BLACK;
				} else {
					m.color = Color::WHITE;
				}
				for (int i = 1; i < size; ++i) {
					m.coord.setFromSgfStr(p[i], boardSize);
					stones.push_back(m);
				}
			}
		}
	}
	return normalize(defaultCharacterSet);
}

bool Game::setHandicapStones(int n, bool jp/* =false */)
{
	if (n < 0 || n == 1 || n > 9) {
		return false;
	}
	const char* handi19[11][10] =
	{
		{},
		{},
		{"D4", "Q16"},
		{"D4", "Q16", "D16"},
		{"D4", "Q16", "D16", "Q4"},
		{"D4", "Q16", "D16", "Q4", "K10"},
		{"D4", "Q16", "D16", "Q4", "D10", "Q10"},
		{"D4", "Q16", "D16", "Q4", "D10", "Q10", "K10"},
		{"D4", "Q16", "D16", "Q4", "D10", "Q10", "K4", "K16"},
		{"D4", "Q16", "D16", "Q4", "D10", "Q10", "K4", "K16", "K10"},
		{"D4", "Q16", "Q4"}
	};
	const char* handi9[11][10] =
	{
		{},
		{},
		{"C3", "G7"},
		{"C3", "G7", "C7"},
		{"C3", "G7", "C7", "G3"},
		{"C3", "G7", "C7", "G3", "E5"},
		{"C3", "G7", "C7", "G3", "C5", "G5"},
		{"C3", "G7", "C7", "G3", "C5", "G5", "E5"},
		{"C3", "G7", "C7", "G3", "C5", "G5", "E3", "E7"},
		{"C3", "G7", "C7", "G3", "C5", "G5", "E3", "E7", "E5"},
		{"C3", "G7", "G3"}
	};

	// 三子の置碁には二通りの初期配置がある
	const char* *h;
	if (boardSize == 19) {
		if (n == 3 && jp) {
			h = handi19[10];
		} else {
			h = handi19[n];
		}
	} else if (boardSize == 9) {
		if (n == 3 && jp) {
			h = handi9[10];
		} else {
			h = handi9[n];
		}
	} else {
		return false;
	}

	handicap = n;
	for (int i = 0; i < n; ++i) {
		Move m;
		m.color = Color::BLACK;
		m.coord.setFromStr(h[i], boardSize);
		stones.push_back(m);
	}

	return true;
}

static const boost::regex reGib(R"((?:\\(\w++)|\\\[(\w++)=(.*?)\\\]|(?!\\)([^\r\n]*+))\r?+\n)");
static const boost::regex reGibPlayer(R"(([^(]*+)\(([^)]*+)\))");
bool Game::readGib(const std::string& gib, const char* defaultCharacterSet)
{
	clear();

	boost::sregex_iterator it(gib.begin(), gib.end(), reGib);
	boost::sregex_iterator itEnd;
	while (it != itEnd && it->str(1) != "HS") {
		++it;
	}
	while (it != itEnd) {
		if (it->str(1) == "HE") {
			++it;
			break;
		}
		const auto s2 = it->str(2);
		const auto s3 = it->str(3);
		if (s2 == "GIBOKIND") {
			if (s3 == "Global") {
				characterSet = "utf-8";
			} else if (s3 == "China") {
				characterSet = "GB2312";
			} else if (s3 == "Korea") {
				characterSet = "EUC-KR";
			} else if (s3 == "Japan") {
				characterSet = "SJIS";
			}
		} else if (s2 == "GAMEPLACE") {
			place = s3;
		} else if (s2 == "GAMEWHITENAME") {
			boost::smatch m;
			if (boost::regex_match(s3, m, reGibPlayer)) {
				whitePlayer = m.str(1);
				whiteRank = m.str(2);
			} else {
				whitePlayer = s3;
			}
		} else if (s2 == "GAMEBLACKNAME") {
			boost::smatch m;
			if (boost::regex_match(s3, m, reGibPlayer)) {
				blackPlayer = m.str(1);
				blackRank = m.str(2);
			} else {
				blackPlayer = s3;
			}
		} else if (s2 == "GAMETAG") {
			std::istringstream tags(s3);
			std::string str;
			std::string winner;
			int count = 0;
			while (std::getline(tags, str, ',')) {
				const auto cstr = str.c_str();
				int n1, n2, n3;
				if (std::sscanf(cstr, "C%d:%d:%d", &n1, &n2, &n3) == 3) {
					std::ostringstream o;
					o << n1;
					o << '-' << std::setw(2) << std::setfill('0') << n2;
					o << '-' << std::setw(2) << std::setfill('0') << n3;
					date = o.str();
				} else if (std::sscanf(cstr, "G%d", &n1) == 1) {
					komi = static_cast<double>(n1) / 10.0;
				} else if (std::sscanf(cstr, "W%d", &n1) == 1) {
					std::ostringstream o;
					switch (n1) {
					case 0:
						winner = "B+";
						break;
					case 1:
						winner = "W+";
						break;
					case 3:
						result = "B+R";
						break;
					case 4:
						result = "W+R";
						break;
					case 7:
						result = "B+T";
						break;
					case 8:
						result = "W+T";
						break;
					case 255:
						// 中断
						break;
					default:
						// 持碁の意味だと思うが未確認
						result = "0";
						break;
					}
				} else if (std::sscanf(cstr, "Z%d", &n1) == 1) {
					count = n1;
				}
			}
			if (!winner.empty()) {
				if (count == 0) {
					result = "0";
				} else {
					std::ostringstream o;
					o << winner << (static_cast<double>(count) / 10.0);
					result = o.str();
				}
			}
		} else if (s2 == "INI") {
		}
		++it;
	}
	while (it != itEnd && it->str(1) != "GS") {
		++it;
	}
	for (int i = 0; i < 3 && it != itEnd; ++i) {
		++it;
	}
	if (it != itEnd) {
		const auto s4 = it->str(4);
		const auto cstr = s4.c_str();
		int n1, n2, n3;
		if (std::sscanf(cstr, "INI %d %d %d", &n1, &n2, &n3) == 3) {
			if (!setHandicapStones(n3)) {
				clear();
				return false;
			}
			++it;
		}
	}
	while (it != itEnd) {
		if (it->str(1) == "GE") {
			return normalize(defaultCharacterSet);
		}
		const auto s4 = it->str(4);
		const auto cstr = s4.c_str();
		int n1, n2, n3, n4, n5;
		if (std::sscanf(cstr, "STO %d %d %d %d %d", &n1, &n2, &n3, &n4, &n5) == 5) {
			Move m;
			m.color = n3 == 1 ? Color::BLACK : Color::WHITE;
			m.coord.setColumnAndRow(n4 + 1, n5 + 1, boardSize);
			moves.push_back(m);
		}
		++it;
	}
	clear();
	return false;
}

static const boost::regex reNgf(R"(((PM|GI)?+([^\r\n]*+))\r?+\n)");

// 以下のboost::regexがDEBUGビルドでかつ対象の文字列がShift_JISのときに
// 正しく働かないがstd::regexならば大丈夫なのでboost::regexのバグのようだ
// 心配なのでとりあえずregexを使用しないように書き直した
//static const boost::regex reNgfPlayer(R"((\S++(?:\s++\S++)*)\s++(\S*+))");
//static const std::regex sreNgfPlayer(R"((\S+(?:\s+\S+)*)\s+(\S*))");

static const boost::regex reNgfDate(R"((\d\d\d\d)(?:(\d\d)(\d\d)|-(\d\d?+)-(\d\d?+)).*+)");
static const boost::regex reNgfResult(R"((?:(Black wins|White loses)|(?:White wins|Black loses))\s++\S++\s++(?:(\d++(?:\.\d++)?+)|([A-Za-z]++)).*+)");
bool Game::readNgf(const std::string& ngf, const char* defaultCharacterSet)
{
	clear();

	boost::sregex_iterator it(ngf.begin(), ngf.end(), reNgf);
	boost::sregex_iterator itEnd;

	std::string ngfBoardSize, ngfWhitePlayer, ngfBlackPlayer,
		ngfHandicap, ngfKomi, ngfDate, ngfResult;

	// NGFファイルの入手が困難なため一部未確認のままになっている
	if (it != itEnd && it->str(2) == "GI") {
		int n = 1;
		while (it != itEnd && it->str(2) == "GI") {
			switch (n) {
			case 2:
				ngfWhitePlayer = it->str(3);// 未確認
				break;
			case 3:
				ngfBlackPlayer = it->str(3);// 未確認
				break;
			case 6:
				ngfDate = it->str(3);
				break;
			case 7:
				ngfBoardSize = it->str(3);
				break;
			case 8:
				ngfHandicap = it->str(3);// 未確認
				break;
			case 10:
				ngfKomi = it->str(3);
				break;
			// Resultは不明である
			}
			++n;
			++it;
		}
		while (it != itEnd && it->str(2) != "PM") {
			++it;
		}
	} else {
		int n = 1;
		while (it != itEnd && it->str(2) != "PM") {
			switch (n) {
			case 2:
				ngfBoardSize = it->str(1);
				break;
			case 3:
				ngfWhitePlayer = it->str(1);
				break;
			case 4:
				ngfBlackPlayer = it->str(1);
				break;
			case 6:
				ngfHandicap = it->str(1);
				break;
			case 8:
				ngfKomi = it->str(1);
				break;
			case 9:
				ngfDate = it->str(1);
				break;
			case 11:
				ngfResult = it->str(1);
				break;
			}
			++n;
			++it;
		}
	}
	if (!ngfBoardSize.empty()) {
		try {
			boardSize = std::stoi(ngfBoardSize);
		} catch (...) {
			clear();
			return false;
		}
	}

	if (!ngfWhitePlayer.empty()) {
		std::tie(whitePlayer, whiteRank) = SplitByLastSpace(ngfWhitePlayer);
	}
	if (!ngfBlackPlayer.empty()) {
		std::tie(blackPlayer, blackRank) = SplitByLastSpace(ngfBlackPlayer);
	}
	if (!ngfHandicap.empty()) {
		try {
			if (!setHandicapStones(std::stoi(ngfHandicap))) {
				clear();
				return false;
			}
		} catch (...) {
			clear();
			return false;
		}
	}
	if (!ngfKomi.empty()) {
		try {
			komi = static_cast<double>(std::stoi(ngfKomi));
			if (handicap == 0) {
				komi += 0.5;
			}
		} catch (...) {
			clear();
			return false;
		}
	}
	if (!ngfDate.empty()) {
		boost::smatch m;
		if (boost::regex_match(ngfDate, m, reNgfDate)) {
			std::ostringstream o;
			o << m.str(1) << '-';
			if (!m.str(2).empty()) {
				o << m.str(2) << '-' << m.str(3);
			} else {
				o << '-' << std::setw(2) << std::setfill('0') << m.str(4);
				o << '-' << std::setw(2) << std::setfill('0') << m.str(5);
			}
			date = o.str();
		}
	}
	if (!ngfResult.empty()) {
		boost::smatch m;
		if (boost::regex_match(ngfResult, m, reNgfResult)) {
			std::ostringstream o;
			if (!m.str(1).empty()) {
				o << "B+";
			} else {
				o << "W+";
			}
			if (!m.str(2).empty()) {
				o << m.str(2);
				result = o.str();
			} else {
				auto s = m.str(3);
				if (s.length() > 0) {
					if (s[0] == 'R' || s[0] == 'r') {
						o << 'R';
					} else if (s[0] == 'T' || s[0] == 't'){
						o << 'T';
					} else {
						o << s;
					}
					result = o.str();
				}
			}
		}
	}
	while (it != itEnd) {
		if (it->str(2) == "PM") {
			auto s = it->str(3);
			if (s.length() >= 3) {
				if (s[2] == 'B' || s[2] == 'W') {
					Move m;
					if (s[2] == 'B') {
						m.color = Color::BLACK;
					} else {
						m.color = Color::WHITE;
					}
					if (s.length() >= 5) {
						m.coord.setColumnAndRow(static_cast<int>(s[3] - 'A'), static_cast<int>(s[4] - 'A'), boardSize);
					} else {
						m.coord.setPass();
					}
					moves.push_back(m);
				}
			}
		}
		++it;
	}
	return normalize(defaultCharacterSet);
}

static const boost::regex reUgf(R"((?:\[([^]]++)\]|(?:(\w++)=)?+([^\r\n]*+))\r?+\n)");
static const boost::regex reUgfResult(R"((\w),(?:(\d++)(\.\d++)?+|([A-Z])).*+)");
bool Game::readUgf(const std::string& ugf, const char* defaultCharacterSet)
{
	clear();

	boost::sregex_iterator it(ugf.begin(), ugf.end(), reUgf);
	boost::sregex_iterator itEnd;

	while (it != itEnd && it->str(1) != "Header") {
		++it;
	}
	if (it == itEnd) {
		clear();
		return false;
	}
	++it;
	while (it != itEnd && it->str(1).empty()) {
		auto s2 = it->str(2);
		auto s3 = it->str(3);
		if (s2 == "Size") {
			try {
				boardSize = std::stoi(s3);
			} catch (...) {
				clear();
				return false;
			}
		} else if (s2 == "Lang") {
			characterSet = s3; // SJIS以外は見たことがない
		} else if (s2 == "PlayerB") {
			std::istringstream is(s3);
			std::string name, rank;
			if (std::getline(is, name, ',') && std::getline(is, rank, ',')) {
				blackPlayer = name;
				blackRank = rank;
			}
		} else if (s2 == "PlayerW") {
			std::istringstream is(s3);
			std::string name, rank;
			if (std::getline(is, name, ',') && std::getline(is, rank, ',')) {
				whitePlayer = name;
				whiteRank = rank;
			}
		} else if (s2 == "Place") {
			place = s3;
		} else if (s2 == "Winner") {
			boost::smatch m;
			if (boost::regex_match(s3, m, reUgfResult)) {
				auto m1 = m.str(1);
				auto m2 = m.str(2);
				auto m3 = m.str(3);
				auto m4 = m.str(4);
				if (m1 == "B") {
					if (m2.empty()) {
						if (!m4.empty()) {
							switch (m4[0]) {
							case 'C': case 'G': case 'E':
								result = "B+R";
								break;
							case 'F':
								result = "B+F";
								break;
							case 'T':
								result = "B+T";
								break;
							}
						}
					} else {
						result = std::string("B+") + m2;
						if (!m3.empty() && m3 != ".0") {
							result += m3;
						}
					}
				} else if (m1 == "W") {
					if (m2.empty()) {
						if (!m4.empty()) {
							switch (m4[0]) {
							case 'C': case 'G': case 'E':
								result = "W+R";
								break;
							case 'F':
								result = "W+F";
								break;
							case 'T':
								result = "W+T";
								break;
							}
						}
					} else {
						result = std::string("W+") + m2;
						if (!m3.empty() && m3 != ".0") {
							result += m3;
						}
					}
				} else if (m1 == "D") {
					result = "0";
				}
			}
		} else if (s2 == "Rule") {
			if (s3 == "JPN") {
				ruleset = "Japanese";
			} else if (s3 == "CHN") {
				ruleset = "Chinese";
			} else {
				ruleset = s3;
			}
		} else if (s2 == "Hdcp") {
			int ugfHandicap;
			double ugfKomi;
			if (sscanf(s3.c_str(), "%d,%lf", &ugfHandicap, &ugfKomi) == 2) {
				handicap = ugfHandicap;
				komi = ugfKomi;
			}
		} else if (s2 == "Date") {
			std::istringstream is(s3);
			std::string ugfDate;
			if (std::getline(is, ugfDate, ',')) {
				date = ugfDate;
			}
		}
		++it;
	}
	while (it != itEnd && it->str(1) != "Data") {
		++it;
	}
	if (it == itEnd) {
		clear();
		return false;
	}
	++it;
	while (it != itEnd && it->str(1).empty()) {
		char c1, c2, c3, c4;
		int d5;
		int h = handicap;
		if (sscanf(it->str(3).c_str(), "%c%c,%c%c,%d", &c1, &c2, &c3, &c4, &d5) == 5) {
			Move m;
			m.color = (c3 == 'B' ? Color::BLACK : Color::WHITE);
			m.coord.setColumnAndRow((static_cast<int>(c1 - 'A') + 1), (static_cast<int>(c2 - 'A') + 1), boardSize);
			if (h == 0) {
				moves.push_back(m);
			} else {
				// 置碁のときは、着手リストから置き石の数だけ初期配置に追加
				stones.push_back(m);
				--h;
			}
		}
		++it;
	}

	return normalize(defaultCharacterSet);
}

std::string Game::sgfStr() const
{
	std::ostringstream o;
	o << "(;";
	if (!characterSet.empty()) {
		o << "CA[" << characterSet << ']';
	}
	o << "GM[1]FF[4]";
	o << "SZ[" << boardSize << ']';
	if (!blackPlayer.empty()) {
		o << "PB[" << Escape(blackPlayer, ']') << ']';
	}
	if (!blackRank.empty()) {
		o << "BR[" << Escape(blackRank, ']') << ']';
	}
	if (!whitePlayer.empty()) {
		o << "PW[" << Escape(whitePlayer, ']') << ']';
	}
	if (!whiteRank.empty()) {
		o << "WR[" << Escape(whiteRank, ']') << ']';
	}
	if (!result.empty()) {
		o << "RE[" << Escape(result, ']') << ']';
	}
	o << "KM[" << komi << ']';
	if (handicap > 0) {
		o << "HA[" << handicap << ']';
	}
	if (!date.empty()) {
		o << "DT[" << Escape(date, ']') << ']';
	}
	std::ostringstream ob, ow;
	for (const auto& m : stones) {
		if (m.color == Color::BLACK) {
			ob << '[' << m.coord.sgfStr() << ']';
		} else if (m.color == Color::WHITE) {
			ow << '[' << m.coord.sgfStr() << ']';
		}
	}
	std::string ab = ob.str();
	std::string aw = ow.str();
	if (!ab.empty()) {
		o << "AB" << ab;
	}
	if (!aw.empty()) {
		o << "AW" << aw;
	}
	for (const auto& m : moves) {
		o << ((m.color == Color::BLACK) ? ";B[" : ";W[") << m.coord.sgfStr() << ']';
	}
	o << ')';
	return o.str();
}

std::string Game::ngfStr() const
{
	std::ostringstream o;
	o << '\n';
	o << boardSize << '\n';
	o << whitePlayer;
	for (int i = 11 - static_cast<int>(whitePlayer.length()); i > 0; --i) {
		o << ' ';
	}
	o << whiteRank << " \n";
	o << blackPlayer;
	for (int i = 11 - static_cast<int>(blackPlayer.length()); i > 0; --i) {
		o << ' ';
	}
	o << blackRank << " \n";
	o << "www.example.com\n";
	o << handicap << '\n';
	o << "0\n";
	int k = static_cast<int>(komi);
	o << k << '\n';
	o << date << '\n';
	o << "5\n";
	if (result.length() > 3U) {
		if (result[0] == 'B') {
			if (result[2] == 'R') {
				o << "Black wins by resignation!\n";
			} else if (result[2] == 'T') {
				o << "White loses on time!\n";
			} else {
				try {
					double p = std::stod(result.substr(2U));
					o << "Black wins by " << p << " points!\n";
				} catch (...) {
					o << "Black wins!\n";
				}
			}
		} else if (result[0] == 'W') {
			if (result[2] == 'R') {
				o << "White wins by resignation!\n";
			} else if (result[2] == 'T') {
				o << "Black loses on time!\n";
			} else {
				try {
					double p = std::stod(result.substr(2U));
					o << "White wins by " << p << " points!\n";
				} catch (...) {
					o << "White wins!\n";
				}
			}
		} else {
			o << '\n';
		}
	} else {
		o << result << '\n';
	}
	o << moves.size() << '\n';
	int count = 1;
	for (const auto& m : moves) {
		o << "PM";
		o << static_cast<char>('A' + count / 26);
		o << static_cast<char>('A' + count % 26);
		o << ((m.color == Color::BLACK) ? 'B' : 'W');
		if (m.coord.isPass()) {
			o << "AAAA\n";
		} else {
			const char r = static_cast<char>('A' + m.coord.row);
			const char c = static_cast<char>('A' + m.coord.column);
			o << c << r << r << c << '\n';
		}		
		++count;
	}
	return o.str();
}

// 棋譜上で着手番号100と表示される手がmoves[99]になる
// 最後の着手番号が100である局面のときcurrentPosition==100になる
bool Game::play(GtpLz& gtp)
{
	std::ostringstream gtpCommand;
	if (currentPosition < static_cast<int>(moves.size())) {
		const auto m = moves[currentPosition];
		gtpCommand << "play " << (m.color == Color::BLACK ? "b " : "w ") << m.coord.str(boardSize);
		gtp.send(gtpCommand.str());
		std::string line = gtp.getResult();
		if (!line.empty() && line.front() == '=') {
			++currentPosition;
			return true;
		} else {

			// 不正な手の時は代わりにパスをして局面の番号がずれないようにする
			gtp.send(m.color == Color::BLACK ? "play b pass" : "play w pass");
			gtp.getResult();
			++currentPosition;
			return true;
		}
	}
	return false;
}

bool Game::undo(GtpLz& gtp)
{
	if (currentPosition >= 1) {
		gtp.send("undo");
		const auto line = gtp.getResult();
		if (!line.empty() && line.front() == '=') {
			--currentPosition;
			return true;
		} else {

			// undoに失敗することはあり得ないが
			// 一応、局面の番号がずれないようにする
			--currentPosition;
			return false;
		}
	}
	return false;
}

bool Game::go(int position, GtpLz& gtp)
{
	// boardsizeコマンドを送ると同時にclear_boardコマンドも実行される
	gtp.send(std::string("boardsize ") + std::to_string(boardSize));
	std::string response = gtp.getResult();
	if (response.empty() || response.front() == '?') {
		return false;
	}
	std::ostringstream ok;
	ok << "komi " << komi;
	gtp.send(ok.str());
	gtp.getResult();
	
	// 持ち時間は仮に1000時間としておく
	gtp.send("time_settings 3600000 0 0");
	gtp.getResult();

	for (const auto& move : stones) {
		if (move.color == Color::BLACK) {
			gtp.send(std::string("play b ") + move.coord.str(boardSize));
			gtp.getResult();
		} else {
			gtp.send(std::string("play w ") + move.coord.str(boardSize));
			gtp.getResult();
		}
	}

	currentPosition = 0;
	for (int i = 0; i < position; ++i) {
		if (!play(gtp)) {
			return false;
		}
	}
	return true;
}

// 前方へ進むときは初期設定をしないで局面の移動をする
bool Game::moveTo(int position, GtpLz& gtp)
{
	// 現在の局面より後方の局面へ移動するときは棋譜を初めから並べなおす
	if (currentPosition > position) {
		if (!go(position, gtp)) {
			return false;
		}
	} else {
		for (int i = currentPosition; i < position; ++i) {
			if (!play(gtp)) {
				return false;
			}
		}
	}
	return true;
}
