#include "variation_list.h"

// genmoveの出力をそのまま読む
bool VariationList::create(GtpLz& gtp, Color color, int boardSize)
{

	// 「Thinking」で始まる行まで読み飛ばす
	// この処理はなくてもよいが、Leela Zeroの出力する文字列が
	// 変更になっても正しく動作する可能性が若干上がるので追加した。
	gtp.getLine("Thinking");
	gtp.skipNextBlankLine();

	variations.clear();
	Variation v;
	while (v.create(gtp, color, boardSize)) {
		variations.push_back(v);
	}
	gtp.skipNextBlankLine();
	return true;
}
