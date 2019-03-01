#include <chess/square.h>

#include <stdexcept>

using chess::Square;
using chess::Colour;
using chess::Empty;

Colour chess::get_colour(Square sq)
{
    return sq.colour();
}

Colour chess::flip_colour(Colour colour)
{
    return (colour == Colour::white) ? Colour::black : Colour::white;
}

bool chess::is_colour(Square sq, Colour c)
{
    return sq.type() != SquareType::empty && get_colour(sq) == c;
}

bool chess::has_moved(Square sq)
{
    return sq.has_moved();
}

void chess::set_moved(Square & sq)
{
    sq.set_moved();
}