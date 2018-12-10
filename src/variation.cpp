#include "variation.h"
#include "log_file.h"

#include <boost/regex.hpp>

static const boost::regex reVariation(R"( *(\w++) -> +(\d++)\D++(\d++\.\d++)\D++(\d++\.\d++)\W++PV\W++(.++))");
static const boost::regex reCoordinate(R"(\w++)");

bool Variation::create(GtpLz& gtp, Color color, int boardSize)
{
	boost::smatch what;
	const auto line = gtp.getLine();
	if (boost::regex_match(line, what, reVariation)) {
		move.color = color;
		move.coord.setFromStr(what[1].str(), boardSize);
		playout = std::stoi(what[2].str());
		v = std::stod(what[3].str());
		n = std::stod(what[4].str());
		pv.clear();
		Move m;
		m.color = move.color;
		const auto coord = what[5].str();
		boost::sregex_iterator it(coord.begin(), coord.end(), reCoordinate);
		boost::sregex_iterator itEnd;
		while (it != itEnd) {
			m.coord.setFromStr((*it)[0].str(), boardSize);
			pv.push_back(m);
			if (m.color == Color::BLACK) {
				m.color = Color::WHITE;
			} else {
				m.color = Color::BLACK;
			}
			++it;
		}
		return true;
	}
	return false;
}
