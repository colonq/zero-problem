#include "leela_zero.h"
#include "log_file.h"
#include "exit_status.h"

#include <boost/regex.hpp>
#include <boost/filesystem.hpp>

#ifdef WIN32
#include <boost/process/windows.hpp>
#endif

static const boost::regex reFirstLine(R"(Using \d++ thread\(s\)\..*+)");
LeelaZero::LeelaZero(const String& lz, const String& networkWeights, int nThreads, std::atomic<bool>& ss)
	: shouldStop(ss)
{
	if (!boost::filesystem::exists(lz)
		|| !boost::filesystem::exists(networkWeights)) {
		throw ExitStatus::FAILED;
	}

	// networkWeightsのファイルサイズが公開されている0番のファイルより小さいならば
	// 確実にパスを間違えている
	bool tooSmall = false;
	try {
		if (boost::filesystem::file_size(networkWeights) < 1373631U) {
			tooSmall = true;
		}
	} catch (...) {
		throw ExitStatus::FAILED;
	}
	if (tooSmall) {
		throw ExitStatus::FAILED;
	}

	String t = ToString(std::to_string(nThreads));
	String command = ToString("\"") + lz
		+ ToString("\" -g -w \"") + networkWeights
		+ ToString("\" -r 0 -t ") + t
		+ ToString(" --noponder");

	Log("<Command>");
	Log(ToUtf8String(command));
	Log("");

	child = new boost::process::child(
		command,
		boost::process::std_in < ops,
		boost::process::std_out > ips,
		boost::process::std_err > eps
#ifdef WIN32
		, boost::process::windows::hide
#endif
	);
	if (!child->running()) {
		delete child;
		throw ExitStatus::FAILED;
	}

	// 設定に間違ったプログラムのパスが書かれているときに
	// このプログラムがフリーズしないようにする
	// その間違って設定されたプログラムが--noponder等の不明なオプションから
	// 何らかのエラーメッセージを出力することを期待して
	// 標準エラー出力の最初の一行を読んでLeela Zeroか判定する
	Log("<Information>");
	const auto firstLine = getLine();
	if (!boost::regex_match(firstLine, reFirstLine)) {
		throw ExitStatus::FAILED;
	}

	// ログが読みにくくなるので起動時の出力はすべて読んでおく
	getLine("Setting");
	Log("");
}

LeelaZero::~LeelaZero()
{
	delete child;
}

std::string LeelaZero::getLine()
{
	if (shouldStop.load()) {
		throw ExitStatus::CANCELED;
	}
	std::string line;
	if (eps && std::getline(eps, line)) {
		Trim(line, '\r');
		Log(line);
		return line;
	}
	return std::string();
}

std::string LeelaZero::getLine(const std::string& s)
{
	std::string line;
	while (eps) {
		line = getLine();
		if (s.empty() || line.substr(0U, s.length()) == s) {
			return line;
		}
	}
	return std::string();
}

std::string LeelaZero::getResult()
{
	if (shouldStop.load()) {
		throw ExitStatus::CANCELED;
	}
	std::string line;
	std::ostringstream result;
	while (ips && std::getline(ips, line) && !line.empty() && line.front() != '\r') {
		Trim(line, '\r');
		Log(line);
		result << line << '\n';
	}
	Log("");

	return result.str();
}

void LeelaZero::send(const std::string& gtpCommand)
{
	if (ops) {
		Log(gtpCommand);
		ops << gtpCommand << std::endl;
	}
}
