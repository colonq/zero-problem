#include "job.h"
#include "string_util.h"
#include "log_file.h"

#include <fstream>
#include <iterator>
#include <algorithm>

void Job::clear()
{
	name.clear();
	player = Player::NONE;
	numberOfProblems = 0;
	start = 0;
	last = 0;
	doubleCountOfProblems = 0;
	position = 0;
	updated = false;
	game.clear();
}

bool Job::isUnnecessary() const
{
	return player == Player::NONE || numberOfProblems == 0;
}

void Job::setPlayer(const Setting& setting)
{
	// 設定の碁盤のサイズと違えば問題作成はしない
	// 現在は19路盤専用だが将来サイズを変更できるようになったときのために
	// 設定から碁盤のサイズを参照している
	if (game.boardSize != setting.boardSize) {
		player = Player::NONE;
		return;
	}

	// 手番で問題生成の対象が指定されている場合はそのまま採用する
	player = setting.player;
	if (player != Player::BY_NAME) {
		return;
	}

	// プレイヤー名で問題生成の対象が指定されている場合は
	// 棋譜のプレイヤー名と設定のプレイヤー名のリストを一つずつ比較していく
	// (大文字小文字の違いは区別される)
	std::string name;
	bool isBlack = false;
	bool isWhite = false;
	for (const auto& p : setting.playerNames) {
		name = ToUtf8String(p);
		if (name == game.blackPlayer) {
			isBlack = true;
		} else if (name == game.whitePlayer) {
			isWhite = true;
		}
	}
	if (isBlack) {
		if (isWhite) {
			player = Player::BOTH;
		} else {
			player = Player::BLACK;
		}
	} else if (isWhite) {
		player = Player::WHITE;
	} else {
		player = Player::NONE;
	}
}

void Job::setNumberOfProblems(const Setting& setting)
{
	setRange(setting);

	// 短手数の棋譜では少数の問題作成で十分なので
	// 棋譜内の問題作成の対象となる手数bと
	// 設定上での問題作成の対象となる手数aとを比較して
	// 実際に何問作成するか決定する
	// (aが210を超えるときはa == 210と解釈する)
	const int b = (last + 1) - start;
	const int a = std::min((setting.last + 1) - setting.start, 210);
	if (b <= 0 || a <= 0) {
		numberOfProblems = 0;
		return;
	}

	// 設定上の問題数にb/aをかけたものに1を足したものが問題数
	int n = ((setting.numberOfProblems * b) / a) + 1;

	// ただし、設定上の問題数を超えることは許されない
	n = std::min(n, setting.numberOfProblems);

	// さらに、手数bを超えることはありえない
	n = std::min(n, b);

	Log("<NumberOfProblems>");
	Log(std::to_string(setting.numberOfProblems) + " -> " + std::to_string(n));
	Log("");

	numberOfProblems = n;
}

void Job::setRange(const Setting& setting)
{
	start = setting.start;
	last = std::min(setting.last, static_cast<int>(game.moves.size()));
}

int Job::getProgressRate(const Setting& setting) const
{
	const int q1 = setting.getVirtualNumberOfVisits1();
	const int q2 = setting.getVirtualNumberOfVisits2();
	int a = 0;

	// 必要訪問数
	// positionの最大値はlast + 1
	if (start >= 1 && last - start >= 0) {
		a += ((last + 1) - (start - 1)) * q1;
		if (numberOfProblems > 0) {
			a += numberOfProblems * q2 * 2;
		}
	}
	if (a == 0) {
		return 0;
	}

	// 完了訪問数
	int b = 0;
	if (start >= 1 && position - start >= 0) {
		b += (position - (start - 1)) * q1;
		if (doubleCountOfProblems > 0) {
			b += doubleCountOfProblems * q2;
		}
	}

	// 単位が%なので100倍する
	const int progressRate = (b * 100) / a;
	return progressRate;
}

bool Job::read(const String& filename, const FileType type, const Setting& setting)
{
	clear();

	std::ifstream ifs(filename);
	if (!ifs) {
		name.clear();
		return false;
	}
	name = filename;

	// ほとんどの人が使用しない項目を設定に入れるのは避けたいので
	// ディフォルトの文字コードはUTF-8/日本語/中国語/韓国語の４種類しか扱わない
	// 特殊なファイルを読みたい人は他のアプリケーションで変換してから
	// 読み込んでもらうことにする
	const char* dcs = "UTF-8";
	switch (setting.dafaultCharacterSet) {
	case CharacterSet::UTF8:
		dcs = "UTF-8";
		break;
	case CharacterSet::SHIFT_JIS:
		dcs = "Shift_JIS";
		break;
	case CharacterSet::GB2312:
		dcs = "GB2312";
		break;
	case CharacterSet::EUC_KR:
		dcs = "EUC_KR";
		break;
	}

	// ファイルの全ての内容を文字列にコピーしてからgameに読み込む
	std::string s((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
	if ((type == FileType::SGF && game.readSgf(s, dcs))
		|| (type == FileType::GIB && game.readGib(s, dcs))
		|| (type == FileType::NGF && game.readNgf(s, dcs))
		|| (type == FileType::UGF && game.readUgf(s, dcs))
		|| (type == FileType::UNKNOWN && game.readSgf(s, dcs))) {
		setPlayer(setting);
		setNumberOfProblems(setting);
		return true;
	}
	return false;
}
