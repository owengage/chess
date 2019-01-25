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
}

Colour chess::get_colour(Square sq)
{
    return std::visit(ColourVisitor{}, sq);
}