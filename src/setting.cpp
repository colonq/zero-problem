#include "setting.h"

#include <fstream>
#include <sstream>
#include <algorithm>

// 設定ファイルの文字コードはUTF-8で、その書式は各項目ごとに一行で
// keyとvalueを半角スペース一個で区切って並べていくだけの単純なものになる

static const String GetSettingFileName()
{
#ifdef _WIN32
	// 開き方がわかりやすいように拡張子はTXTにする
	return ToString("settings.txt");
#else
	return ToString(getConfigDir() + "/.zproblem");
#endif
}

void Setting::setDefaultValues()
{
	lz.clear();
	networkWeights.clear();
	numberOfThreads = 2;
	numberOfVisits = 300;
	numberOfProblems = 9;
	start = 5;
	last = 244;
	maximumNumberOfMoves = 30;
	player = Player::BY_NAME;
	playerNames.clear();
	dafaultCharacterSet = CharacterSet::UTF8;
	searchSpeed = 0.15;
	logging = false;
	boardSize = 19;
}

int Setting::getNumberOfVisitsInTheFirstStep() const
{
	return std::max(numberOfVisits / 30, 1);
}

int Setting::getVirtualNumberOfVisits1() const
{
	return getNumberOfVisitsInTheFirstStep() + 3;
}

int Setting::getVirtualNumberOfVisits2() const
{
	return numberOfVisits - numberOfVisits / 10;
}

static const char* const KEY_FORMAT = "Format";
static const char* const KEY_LZ = "LeelaZero";
static const char* const KEY_NETWORK_WEIGHTS = "NetworkWeights";
static const char* const KEY_NUMBER_OF_THREADS = "NumberOfThreads";
static const char* const KEY_NUMBER_OF_VISITS = "NumberOfVisits";
static const char* const KEY_NUMBER_OF_PROBLEMS = "NumberOfProblems";
static const char* const KEY_START = "Start";
static const char* const KEY_LAST = "Last";
static const char* const KEY_MAXIMUM_NUMBER_OF_MOVES = "MaximumNumberOfMoves";
static const char* const KEY_PLAYER = "Player";
static const char* const KEY_PLAYER_NAMES = "PlayerNames";
static const char* const KEY_CHARACTER_SET = "CharacterSet";
static const char* const KEY_SEARCH_SPEED = "SearchSpeed";
static const char* const KEY_LOGGING = "Logging";
static const char* const KEY_BOARD_SIZE = "BoardSize";

static const char* const VALUE_BLACK = "black";
static const char* const VALUE_WHITE = "white";
static const char* const VALUE_BOTH = "both";
static const char* const VALUE_BY_NAME = "by name";
static const char* const VALUE_UTF8 = "UTF-8";
static const char* const VALUE_SHIFT_JIS = "Shift_JIS";
static const char* const VALUE_GB2312 = "GB2312";
static const char* const VALUE_EUC_KR = "EUC-KR";
static const char* const VALUE_ENABLE = "enable";
static const char* const VALUE_DISABLE = "disable";

bool Setting::load()
{
	setDefaultValues();

	std::ifstream ifs(GetSettingFileName());
	if (!ifs) {
		return false;
	}

	try {
		std::string line, key, value;
		while (std::getline(ifs, line)) {
			auto n = line.find(' ');
			if (n != std::string::npos) {
				key = line.substr(0U, n);
				value = line.substr(n + 1);
				if (key == KEY_LZ) {
					lz = ToString(value);
				} else if (key == KEY_NETWORK_WEIGHTS) {
					networkWeights = ToString(value);
				} else if (key == KEY_NUMBER_OF_THREADS) {
					numberOfThreads = std::stoi(value);
				} else if (key == KEY_NUMBER_OF_VISITS) {
					numberOfVisits = std::stoi(value);
				} else if (key == KEY_NUMBER_OF_PROBLEMS) {
					numberOfProblems = std::stoi(value);
				} else if (key == KEY_START) {
					start = std::stoi(value);
				} else if (key == KEY_LAST) {
					last = std::stoi(value);
				} else if (key == KEY_MAXIMUM_NUMBER_OF_MOVES) {
					maximumNumberOfMoves = std::stoi(value);
				} else if (key == KEY_CHARACTER_SET) {
					if (value == VALUE_UTF8) {
						dafaultCharacterSet = CharacterSet::UTF8;
					} else if (value == VALUE_SHIFT_JIS) {
						dafaultCharacterSet = CharacterSet::SHIFT_JIS;
					} else if (value == VALUE_GB2312) {
						dafaultCharacterSet = CharacterSet::GB2312;
					} else if (value == VALUE_EUC_KR) {
						dafaultCharacterSet = CharacterSet::EUC_KR;
					}
				} else if (key == KEY_PLAYER) {
					if (value == VALUE_BLACK) {
						player = Player::BLACK;
					} else if (value == VALUE_WHITE) {
						player = Player::WHITE;
					} else if (value == VALUE_BOTH) {
						player = Player::BOTH;
					} else if (value == VALUE_BY_NAME) {
						player = Player::BY_NAME;
					}
				} else if (key == KEY_PLAYER_NAMES) {
					playerNames.clear();
					std::string name;
					std::istringstream iss(value);
					while (std::getline(iss, name, ';')) {
						playerNames.push_back(ToString(name));
					}
				} else if (key == KEY_SEARCH_SPEED) {
					searchSpeed = std::stod(value);
				} else if (key == KEY_LOGGING) {
					if (value == VALUE_ENABLE) {
						logging = true;
					} else if (value == VALUE_DISABLE) {
						logging = false;
					}
				} else if (key == KEY_BOARD_SIZE) {

					// 現在は19路盤しか対応しないのでvalueは無視する
					boardSize = 19;
				}
			}
		}
	} catch(...) {
		return false;
	}

	return true;
}

bool Setting::save() const
{
	std::ofstream ofs(GetSettingFileName());
	if (!ofs) {
		return false;
	}

	ofs << KEY_FORMAT << " 1\n";
	ofs << KEY_LZ << ' ' << ToUtf8String(lz) << '\n';
	ofs << KEY_NETWORK_WEIGHTS << ' ' << ToUtf8String(networkWeights) << '\n';
	ofs << KEY_NUMBER_OF_THREADS << ' ' << numberOfThreads << '\n';
	ofs << KEY_NUMBER_OF_VISITS << ' ' << numberOfVisits << '\n';
	ofs << KEY_NUMBER_OF_PROBLEMS << ' ' << numberOfProblems << '\n';
	ofs << KEY_START << ' ' << start << '\n';
	ofs << KEY_LAST << ' ' << last << '\n';
	ofs << KEY_MAXIMUM_NUMBER_OF_MOVES << ' ' << maximumNumberOfMoves << '\n';
	ofs << KEY_CHARACTER_SET << ' ';
	switch (dafaultCharacterSet) {
	case CharacterSet::UTF8:
		ofs << VALUE_UTF8;
		break;
	case CharacterSet::SHIFT_JIS:
		ofs << VALUE_SHIFT_JIS;
		break;
	case CharacterSet::GB2312:
		ofs << VALUE_GB2312;
		break;
	case CharacterSet::EUC_KR:
		ofs << VALUE_EUC_KR;
		break;
	}
	ofs << '\n';
	ofs << KEY_PLAYER << ' ';
	switch (player) {
	case Player::BLACK:
		ofs << VALUE_BLACK;
		break;
	case Player::WHITE:
		ofs << VALUE_WHITE;
		break;
	case Player::BOTH:
		ofs << VALUE_BOTH;
		break;
	case Player::BY_NAME:
		ofs << VALUE_BY_NAME;
		break;
	case Player::NONE:
		break;
	}
	ofs << '\n';
	ofs << KEY_PLAYER_NAMES << ' ';
	bool firstTime = true;
	for (const auto& name : playerNames) {
		if (!firstTime) {
			ofs << ';';
		}
		ofs << ToUtf8String(name);
		firstTime = false;
	}
	ofs << '\n';
	ofs << KEY_SEARCH_SPEED << ' ' << searchSpeed << '\n';
	ofs << KEY_LOGGING << ' ';
	ofs << (logging ? VALUE_ENABLE : VALUE_DISABLE) << '\n';

	// 現在は19路盤しか対応しないのでboardSizeは無視する
	ofs << KEY_BOARD_SIZE << ' ' << 19 << '\n';

	return true;
}
