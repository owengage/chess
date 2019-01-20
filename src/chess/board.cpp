#include <chess/board.h>

using chess::Loc;
using chess::Board;
using chess::Square;
using chess::Colour;

namespace
{
    void row_of(Square sq, int row, Board & b)
    {
        for (auto loc : Loc::row(row))
        {
            b[loc] = sq;
        }
    }

    void row_royals(Colour colour, int row, Board & b)
    {
        b[{0, row}] = chess::Rook{colour};
        b[{1, row}] = chess::Knight{colour};
        b[{2, row}] = chess::Bishop{colour};
        b[{3, row}] = chess::Queen{colour};
        b[{4, row}] = chess::King{colour};
        b[{5, row}] = chess::Bishop{colour};
        b[{6, row}] = chess::Knight{colour};
        b[{7, row}] = chess::Rook{colour};
    }
}

Board Board::standard()
{
    Board b;

    row_of(Pawn{Colour::white}, 1, b);
    row_of(Pawn{Colour::black}, 6, b);
    row_royals(Colour::white, 0, b);
    row_royals(Colour::black, 7, b);

    return b;
}

Board Board::blank()
{
    return {};
}

Square & Board::operator[](Loc loc)
{
    return squares[loc.index()];
}

Square const& Board::operator[](Loc loc) const
{
    return squares[loc.index()];
}