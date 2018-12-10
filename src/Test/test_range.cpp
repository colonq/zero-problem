#include <boost/test/unit_test.hpp>

#include "../range.h"
#include <sstream>

static std::string RangeString(int start, int last, int count)
{
	auto list = makeRangeList(start, last, count);
	std::ostringstream o;
	for (const auto& r : list) {
		o << '(' << r.start << ',' << r.last << ')';
	}
	return o.str();
}

BOOST_AUTO_TEST_CASE(Range_Test)
{
	BOOST_CHECK_EQUAL(RangeString(1, 100, 2),
		"(1,100)(1,100)");
	BOOST_CHECK_EQUAL(RangeString(1, 100, 10),
		"(1,33)(34,66)(67,100)(17,50)(51,83)"
		"(1,100)(1,100)(1,100)(1,100)(1,100)");
}

/*
#include <random>
#include <fstream>
#include <iomanip>

void Rand(int n)
{
	std::ofstream f("rand.txt");
	std::random_device dev;
	std::mt19937 mt(dev());
	std::uniform_real_distribution<> rand(0, 100);
	for (int i = 0; i < n; ++i) {
		f << std::fixed << std::setprecision(1) << rand(mt);
		if (i != n - 1) {
			f << ',';
		}
		if (i % 10 == 9 || i == n - 1) {
			f << std::endl;
		} else {
			f << ' ';
		}
	}
}
*/

static std::string KeyPositionString(int start, int last, int count, const std::vector<double>& priorities)
{
	auto pos = findKeyPositions(start, last, count, priorities);
	std::ostringstream o;
	int i = static_cast<int>(pos.size());
	for (const auto& p : pos) {
		--i;
		o << p << (i > 0 ? "," : "");
	}
	return o.str();
}

static const std::vector<double> p_30 = {
	-1.0,
	76.3, 4.0, 87.8, 14.4, 75.4, 79.3, 18.9, 23.2, 80.9, 16.2,
	78.1, 73.8, 31.2, 85.5, 87.0, 42.1, 55.9, 22.5, 98.4, 97.0,
	30.7, 50.0, 27.4, 59.7, 68.1, 68.7, 0.3, 24.5, 60.8, 79.4
};

BOOST_AUTO_TEST_CASE(KeyPosition_Test)
{
	BOOST_CHECK_EQUAL(KeyPositionString(1, 30, 3, p_30),
		"3,19,20");
	BOOST_CHECK_EQUAL(KeyPositionString(1, 30, 5, p_30),
		"3,14,15,19,20");
	BOOST_CHECK_EQUAL(RangeString(4, 29, 10),
		"(4,11)(12,20)(21,29)(8,16)(17,24)"
		"(4,29)(4,29)(4,29)(4,29)(4,29)");
	BOOST_CHECK_EQUAL(KeyPositionString(4, 29, 10, p_30),
		"5,6,9,11,12,14,15,19,20,26");
}
