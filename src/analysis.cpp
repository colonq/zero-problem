#include "analysis.h"
#include "board.h"
#include "illustration.h"
#include "range.h"
#include "html.h"
#include "mutex.h"
#include "log_file.h"

#include <cassert>
#include <thread>

Analysis::Analysis()
	: gtp(nullptr)
{
}

Analysis::~Analysis()
{
}

void Analysis::reset(int numberOfVisits)
{
	gtp->send(std::string("lz-setoption name visits value ") + std::to_string(numberOfVisits));
	gtp->getResult();
	game.go(0, *gtp);
	vll.clear();
}

void Analysis::evaluate(int position)
{
	// 最初の局面から最後の局面の次までが評価の対象になる
	if (position < 1 || position > static_cast<int>(game.moves.size()) + 1) {
		return;
	}

	// 評価する局面まで移動する
	game.moveTo(position - 1, *gtp);

	// vll[position]まで使用できるようにvllを拡張する
	while (static_cast<int>(vll.size()) < position + 1) {
		vll.push_back(std::unique_ptr<VariationList>(nullptr));
	}
	vll[position].reset(new VariationList());

	// vllのインデックスは1から始まりmovesのインデックスは0から始まるので
	// その局面の手番はpositionがら単純に1を引いて、
	// Color color = game.moves[position - 1].color; でよいのだが、
	// 最後の局面の次の局面を評価するときはインデックスが範囲外を指してしまうので
	// 一つ手前の局面の逆の手番としている
	Color color;
	if (position - 1 == static_cast<int>(game.moves.size())){
		// 必ずgeme.moves.size() >= 1Uのはずなので position - 2 >= 0 になっている
		const auto& m = game.moves[position - 2];
		color = OpponentColor(m.color);
	} else {
		const auto& m = game.moves[position - 1];
		color = m.color;
	}

	// 基本的にLeela Zeroから標準エラー出力を読むタイミングで中断が必要か確認しているので
	// genmoveの結果が標準出力にくるのを待っているとLeela Zeroの思考中に中断できなくなる
	// そのためgenmoveコマンドを送信して結果を取得するような関数は作成していない
	if (color == Color::BLACK) {
		gtp->send("genmove b");
	} else {
		gtp->send("genmove w");
	}

	// vll[0]は使用しない。vll[position - 1]ではない
	vll[position]->create(*gtp, color, game.boardSize);

	// 棋譜の中に不正な手はないという扱いなので結果によらす無条件で一手進む
	++game.currentPosition;
	gtp->getResult();

	// LeelaZeroの状態を棋譜に存在する手以外が打たれていないように保っておく
	// undo()は成功すればcurrentPositionを一つ減らす
	game.undo(*gtp);
}

double Analysis::calculatePriority(Color c1, double v1, Color c2, double v2)
{
	// 手番の側からみた勝率に直す
	if (c1 != c2) {
		v2 = 100.0 - v2;
	} else {
		return -1.0;
	}
	
	// 勝率の変化した範囲の中央が50%に近いほど重要
	const double c = (50.0 - std::abs((v1 + v2) * 0.5 - 50.0)) / 50.0;
	// 勝率の変化した幅が広いほど重要
	const double d = v1 - v2;

	const double priority = c * d;
	if (priority < 0.0) {
		return -1.0;
	}
	return priority;
}

std::vector<double> Analysis::measurePriorities(Player player)
{
	Log("<Priorities>");

	std::vector<double> priorities;
	const int size = static_cast<int>(vll.size());
	const int msize = static_cast<int>(game.moves.size());
	if (msize < size - 2) {
		// vllとmovesの内容が対応していないのでエラー
		Log("");
		return priorities;
	}

	// 最初の要素は未使用
	priorities.push_back(-1.0);
	for (int i = 1; i < size - 1; ++i) {
		if (!vll[i] || !vll[i + 1]) {
			priorities.push_back(-1.0);
			continue;
		}
		const auto& vv1 = vll[i]->variations;
		const auto& vv2 = vll[i + 1]->variations;
		if (vv1.empty() || vv2.empty()) {
			priorities.push_back(-1.0);
			continue;
		}
		const auto w1 = vv1.front().v;
		const auto w2 = vv2.front().v;

		// 仮に初手の一手しか着手のない棋譜全体を評価した場合
		// size == 3、msize == 1になっているのでi == 1で一回だけ
		// このループ内が実行される
		// 最後の一手に対してgame.moves[i].colorがダメなのは明白
		const Color& c1 = game.moves[i - 1].color;
		Color c2;
		if (i == msize) {
			c2 = (c1 == Color::BLACK ? Color::WHITE : Color::BLACK);
		} else {
			c2 = game.moves[i].color;
		}

		// 問題の作成対象が黒番と白番のどちらか一方のみの指定されているならば
		// 局面の手番と比較し、手番が同じでなければ対象外に設定する
		if ((player == Player::BLACK && c1 != Color::BLACK)
			|| (player == Player::WHITE && c1 != Color::WHITE)) {
			priorities.push_back(-1.0);
			continue;
		}

		// LeelaZeroの推奨する手と実際に打たれた手が同じならば
		// その局面は問題の作成の対象外に設定する
		if (c1 != c2 && vv1.front().move.coord == game.moves[i - 1].coord) {
			priorities.push_back(-1.0);
			continue;
		}
		priorities.push_back(calculatePriority(c1, w1, c2, w2));

		if (Log.isOpen()) {
			std::ostringstream o;
			o << '#' << i << ": " << w1 << " -> " << (100.0 - w2) << " = " << priorities.back();
			Log(o.str());
		}
	}
	Log("");

	return priorities;
}

std::vector<std::string> Analysis::makeJson(
	const std::string& id, int& number,
	int maximumNumberOfMoves, const std::vector<int>& problemPositions)
{
	std::vector<std::string> json;
	game.go(0, *gtp);

	// リストproblemPositionsにある局面を問題にする
	for (const auto& i : problemPositions) {

		// game.moves[i]はvectorの範囲外への参照かもしれないので注意する

		// 一度バグを入れてしまったので
		// 修正後に用心のためにアサーションを追加した
		assert(static_cast<int>(game.moves.size()) > i - 1);
		assert(static_cast<int>(vll.size()) > i + 1);
		assert(vll[i] && vll[i + 1]);

		// リリースビルドでは、万が一バグがあっても無視する
		if (static_cast<int>(game.moves.size()) <= i - 1) {
			continue;
		}
		if (static_cast<int>(vll.size()) <= i + 1) {
			continue;
		}
		if (!vll[i] || !vll[i + 1]) {
			continue;
		}

		const auto& vv1 = vll[i]->variations;
		const auto& vv2 = vll[i + 1]->variations;
		if (vv1.empty() || vv2.empty()) {

			// ここが実行されるようならばLeela Zeroの仕様だと思われる
			continue;
		}

		const auto& v1 = vv1.front();
		const auto& v2 = vv2.front();
		if (v1.pv.size() < 3U || v2.pv.size() < 2U
			|| v1.move.coord == game.moves[i - 1].coord) {

			// ここが実行されるようならばLeela Zeroの仕様だと思われる
			continue;
		}

		// 棋譜が間違っているか壊れている場合でも、その確認は行っていない
		// 反則な着手であってもそのままLeelaZeroに送り付けている
		while (game.currentPosition < i - 1) {
			game.play(*gtp);
		}

		Illustration il;
		gtp->send("showboard");
		gtp->getResult();
		il.board.create(*gtp);
		il.position = i;
		il.winrate[0] = v1.v;
		il.winrate[1] = 100.0 - v2.v;

		// 現在の局面からのPVと
		// 現在の局面で実際に打たれた手＋次の局面のPV
		// の２つの変化を出力する
		il.moves[0] = v1.pv;
		il.moves[1] = v2.pv;
		il.moves[1].insert(il.moves[1].begin(), game.moves[i - 1]);

		// 変化図の手数を設定された最大手数の範囲内に収める
		// また、問題の局面と手順を最後まで進めた後の局面の手番を同じにしておく
		// 黒番の局面と白番の局面では(手番を考慮しない場合に)一手分だけ
		// 形勢が違って見えるので手番の違う局面を並べて表示するのはよくない
		for (int j = 0; j < 2; ++j) {
			auto& m = il.moves[j];
			while (static_cast<int>(m.size()) > maximumNumberOfMoves) {
				m.pop_back();
			}
			if (m.size() % 2U == 1U) {
				m.pop_back();
			}
		}

		// 出力されたHTMLを後に統合したり編集する際に問題に名前があった方が
		// 便利であろうということで、IDの出力をしている
		json.push_back(il.json(id + "_" + std::to_string(number)));
		++number;
	}
	return json;
}

std::vector<std::string> Analysis::generateProblems(
	const Setting& setting, Job& job, JobList& jobList,
	SearchSpeed& searchSpeed, std::atomic<bool>& ss,
	const std::string& id, int& number, std::set<std::string>& hashes)
{
	// q1とq2は誤差を多少考慮した仮想的な訪問数である
	// 誤差を計算に入れないと残り時間の表示が残念なことになる
	const int vi1 = setting.getNumberOfVisitsInTheFirstStep();
	const int vi2 = setting.numberOfVisits;
	const int q1 = setting.getVirtualNumberOfVisits1();
	const int q2 = setting.getVirtualNumberOfVisits2();

	{
		std::lock_guard<std::mutex> lock(gMutex);
		job.doubleCountOfProblems = 0;
		job.position = 0;
	}

	// ステップ１の開始時刻をログに記録する
	Log.time();

	// 訪問数を設定する
	reset(vi1);

	// 対象範囲の一つ先まで評価する
	for (int i = job.start; i <= job.last + 1; ++i) {
		evaluate(i);
		{
			std::lock_guard<std::mutex> lock(gMutex);
			job.position = i;
			job.updated = true;
			searchSpeed.addNumberOfVisits(q1);
			jobList.setExpectedEndTime(setting, searchSpeed.getSecondPerVisit());
		}
	}
	auto priorities = measurePriorities(job.player);
	auto keyPositions = findKeyPositions(job.start, job.last, job.numberOfProblems, priorities);

	if (Log.isOpen()) {
		Log("<KeyPositions>");
		std::ostringstream o;
		o << job.numberOfProblems << " / (" << job.start << '-' << job.last << ") =";
		for (const auto i : keyPositions) {
			o << ' ' << i;
		}
		o << '\n';
		Log(o.str());
	}

	// 指定された数だけの問題を確保できないこともある
	const int size = static_cast<int>(keyPositions.size());
	if (size < job.numberOfProblems) {
		{
			std::lock_guard<std::mutex> lock(gMutex);
			job.doubleCountOfProblems = (job.numberOfProblems - size) * 2;
			job.updated = true;
		}
	}

	// ステップ２の開始時刻をログに記録する
	Log.time();

	// 訪問数を設定する
	reset(vi2);

	// 実際に問題を作成する局面を記録する
	std::vector<int> problemPositions;

	// 最後に評価された局面の番号を記録する
	int lastEvaluatedPosition = -1;

	for (auto p : keyPositions) {

		// 一回ループが回るごとに必ず
		// job.doubleCountOfProblemsが2だけ増えなければならない

		game.moveTo(p - 1, *gtp);
		Board board;
		gtp->send("showboard");
		gtp->getResult();
		board.create(*gtp);

		const auto& hash = board.hash;
		const auto it = hashes.find(hash);
		Log("<Hash>");
		if (it == hashes.end()) {
			Log(std::string("\"") + hash + "\" was not found.");
			Log("");

			// 問題を生成した局面を記録する
			hashes.insert(hash);
		} else {
			Log(std::string("\"") + hash + "\" was found.");
			Log("");

			// 同じ局面で問題を二度生成しない
			{
				std::lock_guard<std::mutex> lock(gMutex);
				job.doubleCountOfProblems += 2;
				job.updated = true;
			}
			continue;
		}

		if (p != lastEvaluatedPosition) {
			evaluate(p);
			lastEvaluatedPosition = p;
			{
				std::lock_guard<std::mutex> lock(gMutex);
				++job.doubleCountOfProblems;
				job.updated = true;
				searchSpeed.addNumberOfVisits(q2);
				jobList.setExpectedEndTime(setting, searchSpeed.getSecondPerVisit());
			}
		} else {
			std::lock_guard<std::mutex> lock(gMutex);
			++job.doubleCountOfProblems;
			job.updated = true;
		}

		// 最前手と実際に打たれた手が同じなら問題にしない
		const auto& vl = vll[p]->variations;
		const auto movesSize = static_cast<int>(game.moves.size());
		if (!vl.empty() && p <= movesSize) {
			const auto& bestMove = vl[0].move;
			const auto& nextMove = game.moves[p - 1];
			if (Log.isOpen()) {
				Log("<Best/Next>");
				std::ostringstream o;
				o << (bestMove.color == Color::BLACK ? "B:" : "W:");
				o << bestMove.coord.str(setting.boardSize);
				o << (bestMove == nextMove ? " == ": " != ");
				o << (nextMove.color == Color::BLACK ? "B:" : "W:");
				o << nextMove.coord.str(setting.boardSize);
				Log(o.str());
				Log("");
			}
			if (bestMove == nextMove) {
				{
					std::lock_guard<std::mutex> lock(gMutex);
					++job.doubleCountOfProblems;
					job.updated = true;
				}
				continue;
			}
		}

		evaluate(p + 1);
		lastEvaluatedPosition = p + 1;
		{
			std::lock_guard<std::mutex> lock(gMutex);
			++job.doubleCountOfProblems;
			job.updated = true;
			searchSpeed.addNumberOfVisits(q2);
			jobList.setExpectedEndTime(setting, searchSpeed.getSecondPerVisit());
		}

		// 問題の作成に成功した
		problemPositions.push_back(p);
	}

	// 問題を作成していない局面に対応するvllの要素が存在する保証がないので
	// 間違えてkeyPositionsを渡すとバグが発生するので注意する
	return makeJson(id, number, setting.maximumNumberOfMoves, problemPositions);
}

std::string Analysis::generateHtml(
	const Setting& setting, JobList& jobList,
	SearchSpeed& searchSpeed, std::atomic<bool>& ss,
	const std::string& title, const std::string& id, ExitStatus& exitStatus)
{
	// 開始時刻をログに記録
	Log.time();

	std::vector<std::string> problems;
	int number = 1;
	exitStatus = ExitStatus::COMPLETED;
	try {

		// Leela Zeroは バージョン0.16以降、再起動せずに訪問数を
		// 変更できるようになったのでここで起動する
		gtp = std::make_unique<LeelaZero>(setting.lz, setting.networkWeights,
			setting.numberOfThreads, ss);

		// lz-setoptionが使えないならばバージョンが古いか
		// Leela Zeroではないプログラムを実行しようとしている
		gtp->send("known_command lz-setoption");
		if (gtp->getResult() != "= true\n") {
			throw ExitStatus::FAILED;
		}

		// 起動時間は探索速度に含めない
		searchSpeed.clear();

		// 同じ局面で二度問題を作らないためにハッシュを記録する
		std::set<std::string> hashes;

		for (auto&& job : jobList.jobs) {
			if (job.isUnnecessary()) {
				continue;
			}
			// 棋譜はコピーしてから使う
			game = job.game;
			auto p = generateProblems(setting, job, jobList, searchSpeed, ss, id, number, hashes);
			problems.insert(problems.end(), p.begin(), p.end());
		}

		// Leela Zeroの終了
		gtp->send("quit");
		gtp->getResult();
		gtp = nullptr;

	} catch (ExitStatus e) {
		exitStatus = e;
		// 中断した場合でも生成した問題は捨てずに保存したいのでここでreturnしない
	}

	// 終了時刻をログに記録
	Log.time();

	return GetHtml(title, problems, setting.boardSize);
}
