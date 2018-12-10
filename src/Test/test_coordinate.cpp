#include <boost/test/unit_test.hpp>

#include "../coordinate.h"

BOOST_AUTO_TEST_CASE(Coordinate_Test)
{
	Coordinate ab = {1, 2};
	BOOST_CHECK_EQUAL(ab.column, 1);
	BOOST_CHECK_EQUAL(ab.row, 2);
	BOOST_CHECK(ab.setPass().isPass());

	Coordinate aa = {1, 1};
	BOOST_CHECK_EQUAL(aa.sgfStr(), "aa");
	BOOST_CHECK_EQUAL(aa.str(19), "A19");
	BOOST_CHECK_EQUAL(aa.str(9), "A9");
	BOOST_CHECK_EQUAL(Coordinate().setFromSgfStr("aa", 19).sgfStr(), "aa");
	BOOST_CHECK_EQUAL(Coordinate().setFromStr("A19", 19).str(19), "A19");
	BOOST_CHECK_EQUAL(Coordinate().setFromStr("A9", 9).str(9), "A9");

	BOOST_CHECK(Coordinate().setFromStr("H10", 19) == Coordinate().setFromSgfStr("hj", 19));
	BOOST_CHECK(Coordinate().setFromStr("I10", 19).isPass());
	BOOST_CHECK(Coordinate().setFromStr("J10", 19) == Coordinate().setFromSgfStr("ij", 19));

	const Coordinate ss = {19, 19};
	BOOST_CHECK_EQUAL(ss.sgfStr(), "ss");
	BOOST_CHECK_EQUAL(ss.str(19), "T1");
	BOOST_CHECK_EQUAL(Coordinate().setFromSgfStr("ss", 19).sgfStr(), "ss");
	BOOST_CHECK_EQUAL(Coordinate().setFromStr("T1", 19).str(19), "T1");

	const Coordinate bb = {2, 2};
	BOOST_CHECK(Coordinate(bb).setFromSgfStr("a", 19).isPass());
	BOOST_CHECK(Coordinate(bb).setFromSgfStr("aaa", 19).isPass());
	BOOST_CHECK(Coordinate(bb).setFromSgfStr("a`", 19).isPass());
	BOOST_CHECK(Coordinate(bb).setFromSgfStr("`a", 19).isPass());
	BOOST_CHECK(Coordinate(bb).setFromSgfStr("at", 19).isPass());
	BOOST_CHECK(Coordinate(bb).setFromSgfStr("ta", 19).isPass());
	BOOST_CHECK(Coordinate(bb).setFromStr("A", 19).isPass());
	BOOST_CHECK(Coordinate(bb).setFromStr("AA", 19).isPass());
	BOOST_CHECK(Coordinate(bb).setFromStr("0", 19).isPass());
	BOOST_CHECK(Coordinate(bb).setFromStr("T20", 19).isPass());
	BOOST_CHECK(Coordinate(bb).setFromStr("U19", 19).isPass());
	BOOST_CHECK(Coordinate(bb).setFromStr("T0", 19).isPass());
	BOOST_CHECK(Coordinate(bb).setFromStr("@1", 19).isPass());

	Coordinate first = {1, 1};
	auto ad1 = first.getAdjacent(19);
	BOOST_CHECK(ad1[0].isPass() && ad1[1].isPass() && !ad1[2].isPass() && !ad1[3].isPass());
	BOOST_CHECK_EQUAL(ad1[2].str(19), "B19");
	BOOST_CHECK_EQUAL(ad1[3].str(19), "A18");

	Coordinate last = {19, 19};
	auto ad2 = last.getAdjacent(19);
	BOOST_CHECK(!ad2[0].isPass() && !ad2[1].isPass() && ad2[2].isPass() && ad2[3].isPass());
	BOOST_CHECK_EQUAL(ad2[0].str(19), "T2");
	BOOST_CHECK_EQUAL(ad2[1].str(19), "S1");
}

