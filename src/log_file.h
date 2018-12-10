#pragma once

#include <string>
#include <fstream>

struct Logfile
{
	std::ofstream logfile;
	Logfile();
	~Logfile();
	void open(const char* filename);
	void close();
	bool isOpen() const;
	void operator() (const std::string& utf8line);
	void time();
};

// ログへの書き込みはスレッドセーフではない
// 複数スレッドからの書き込みが必要になったら
// Logfileのインスタンスを複数作ればよい 
extern struct Logfile Log;
