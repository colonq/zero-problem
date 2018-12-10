#include "move.h"

void Move::clear()
{
	color = Color::EMPTY;
	coord.setPass();
}

bool Move::operator ==(const Move& move) const
{
	return color == move.color && coord == move.coord;
}

bool Move::operator !=(const Move& coord) const
{
	return ! (*this == coord);
}
