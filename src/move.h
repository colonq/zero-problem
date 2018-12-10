#pragma once

#include "color.h"
#include "coordinate.h"

struct Move
{
	Color color;
	Coordinate coord;

	void clear();

	bool operator ==(const Move& coord) const;
	bool operator !=(const Move& coord) const;
};
