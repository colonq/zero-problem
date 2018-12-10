#pragma once

enum class Color : int
{
	BLACK,
	WHITE,
	EMPTY
};

inline Color OpponentColor(Color c)
{
	if (c == Color::BLACK) {
		return Color::WHITE;
	} else if (c == Color::WHITE) {
		return Color::BLACK;
	}
	return c;
}
