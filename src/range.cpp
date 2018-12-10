#include "range.h"

#include <algorithm>

std::vector<Range> makeRangeList(int start, int last, int count)
{

	// 中盤で大きな戦いが発生している対局などでは
	// その周辺ばかりに作成される問題が集中し
	// 序盤や終盤を完全に無視したりすることもあるようだ
	// それはユーザーの期待する振る舞いと違うか思って少し工夫してみた
	// 多少はマシになるようではあるが好みの問題かもしれない
	std::vector<Range> ranges;
	std::vector<Range> ranges2;
	Range r;
	if (count <= 1) {
		r.start = start;
		r.last = last;
		ranges.push_back(r);
		return ranges;
	}
	int n = (last + 1) - start;

	// 分割数の半分までは均等に範囲を区切る
	const int c = count / 2;
	for (int i = 0; i < c; ++i) {
		r.start = start + (n * i) / (c + 1);
		r.last = start + (n * (i + 2)) / (c + 1) - 1;
		if (i % 2 == 0) {
			ranges.push_back(r);
		} else {
			ranges2.push_back(r);
		}
	}
	ranges.insert(ranges.end(), ranges2.begin(), ranges2.end());

	// 分割数の半分以降は全体を範囲にする
	r.start = start;
	r.last = last;
	for (int i = c; i < count; ++i) {
		ranges.push_back(r);
	}

	return ranges;
}

// 重要度を参考に(start,last)の範囲内からcount個の重要な局面を探す
std::vector<int> findKeyPositions(int start, int last, int count, const std::vector<double>& priorities)
{
	std::vector<double> v(priorities);
	const int size = static_cast<int>(v.size());
	std::vector<int> p;
	if (start < 1 || start > size - 1 || last < 1 || last > size - 1 || start > last) {
		return p;
	}
	for (const auto& r : makeRangeList(start, last, count)) {
		auto it = std::max_element(v.begin() + r.start, v.begin() + r.last + 1);
		if (*it > 0) {
			*it = -1.0;
			p.push_back(it - v.begin());
		}
	}
	std::sort(p.begin(), p.end());
	auto begin = std::unique(p.begin(), p.end());
	p.erase(begin, p.end());
	return p;
}
