#pragma once

#include "setting.h"
#include "game.h"

enum class FileType : int
{
	SGF,
	GIB,
	NGF,
	UGF,
	UNKNOWN
};

struct Job
{
	String name;  // ファイル名
	Player player;   // 黒,白,両方,なし
	int numberOfProblems; // 問題の数
	int start;    // 処理範囲の先頭
	int last;     // 処理範囲の最後
	int doubleCountOfProblems;  // 処理が終わった問題の数
	int position; // 処理が終わった局面の番号
	bool updated; // 更新された時にtrue、progress用
	Game game;    // 棋譜

	void clear();
	bool isUnnecessary() const;
	void setPlayer(const Setting& setting);
	void setNumberOfProblems(const Setting& setting);
	void setRange(const Setting& setting);
	int getProgressRate(const Setting& setting) const;
	bool read(const String& filename, const FileType type, const Setting& setting);
};
