#include <chess/square.h>

#include <stdexcept>

using chess::Square;
using chess::Colour;
using chess::PieceCommon;
using chess::Empty;

namespace
{
    struct ColourVisitor {
        Colour operator()(PieceCommon const &p) {
            return p.colour;
        }

        Colour operator()(Empty)
        {
            throw std::runtime_error{"Tried to get colour of empty square"};
        }
    };

    struct HasMovedVisitor {
        bool operator()(PieceCommon const &p) {
            return p.has_moved;
        }

        bool operator()(Empty)
        {
            throw std::runtime_error{"Tried to check movement of empty square"};
        }
    };
}

Colour chess::get_colour(Square sq)
{
    return std::visit(ColourVisitor{}, sq);
}

Colour chess::flip_colour(Colour colour)
{
    return (colour == Colour::white) ? Colour::black : Colour::white;
}

bool chess::is_colour(Square sq, Colour c)
{
    return !std::holds_alternative<Empty>(sq) && get_colour(sq) == c;
}

bool chess::has_moved(Square sq)
{
    return std::visit(HasMovedVisitor{}, sq);
}