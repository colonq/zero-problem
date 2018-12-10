#include <boost/test/unit_test.hpp>

#include "../board.h"
#include "../gtp_lz_stub.h"

static const char* const testBoard1 =
R"(
Passes: 0            Black (X) Prisoners: 58
White (O) to move    White (O) Prisoners: 5

   a b c d e f g h j k l m n o p q r s t 
19 X . . . X X X X X . X X X . . . X X . 19
18 X X . X X . X . X X X . X X X X O . O 18
17 X . X X . . X X O X X . . . . X O O O 17
16 X X X + . X . X O O X . X X . X X O X 16
15 O O O X X X X X X O X . X . X X X X X 15
14 . . X . . X . . X O O X X X . X X O O 14
13 X X X . X . X X O O X . X . X . O O O 13
12 . O X X X X O O . O X X . X X X O X . 12
11 X X X O O O O O O O X . X . X . X X X 11
10 O X X X O O . X . O X X X . X X X O X 10
 9 O O X X O . O X . O O O O X . X O . O  9
 8 . O X O . O O X X . . . O X X X O O .  8
 7 X O X O O . X . X . . . O O O O X O O  7
 6 X O O X O O O X X . . O O O X O X X X  6
 5 . X X X O O O X O X X . O O X X . O X  5
 4 X . X(X)X O X X O + X O O O O X X X X  4
 3 X X O X X O X X O O X O O X O O X . X  3
 2 O O O X X O X . O . X O O X O X X O X  2
 1 . . X X O O O O O O O . O X X X . X X  1
   a b c d e f g h j k l m n o p q r s t 

Hash: 4B1EE4BAB47799B1 Ko-Hash: E07A463A584A8C83

Black time: 01:00:00
White time: 01:00:00

)";


BOOST_AUTO_TEST_CASE(Board_Test)
{
	Board board;
	std::string s1(testBoard1);
	s1 += "END";
	GtpLzStub gtp1(s1);
	BOOST_CHECK(board.create(gtp1));
	BOOST_CHECK_EQUAL(gtp1.getLine(), "END");
	BOOST_CHECK_EQUAL(board.size, 19);
	BOOST_CHECK(board.lastMove.color == Color::BLACK);
	BOOST_CHECK_EQUAL(board.lastMove.coord.str(19), "D4");
	BOOST_CHECK(board.stones[0][0] == Color::BLACK);
	BOOST_CHECK(board.stones[0][18] == Color::EMPTY);
	BOOST_CHECK(board.stones[9][9] == Color::WHITE);
	BOOST_CHECK(board.stones[18][18] == Color::BLACK);
}
