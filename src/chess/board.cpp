#include <chess/board.h>
#include <chess/move.h>

using chess::Loc;
using chess::Move;
using chess::Board;
using chess::Square;
using chess::Colour;
using chess::SquareType;

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
        b[{0, row}] = chess::Rook(colour);
        b[{1, row}] = chess::Knight(colour);
        b[{2, row}] = chess::Bishop(colour);
        b[{3, row}] = chess::Queen(colour);
        b[{4, row}] = chess::King(colour);
        b[{5, row}] = chess::Bishop(colour);
        b[{6, row}] = chess::Knight(colour);
        b[{7, row}] = chess::Rook(colour);
    }

    bool is_pawn_double_jump(Move const& move)
    {
        auto const& sq = move.result[move.dest];
        auto const dy = move.src.y() - move.dest.y();

        return (sq.type() == SquareType::pawn) && std::abs(dy) == 2;
    }
}

Board Board::standard()
{
    Board b;

    row_of(Pawn(Colour::white), 1, b);
    row_of(Pawn(Colour::black), 6, b);
    row_royals(Colour::white, 0, b);
    row_royals(Colour::black, 7, b);

    return b;
}

Board Board::blank()
{
    return {};
}

Board Board::with_pieces(std::vector<std::pair<Loc, Square>> const& pieces)
{
    auto b = Board::blank();
    for (auto const& [loc, sq] : pieces)
    {
        b[loc] = sq;
    }
    return b;
}

Square & Board::operator[](Loc loc)
{
    return squares[loc.index()];
}

Square const& Board::operator[](Loc loc) const
{
    return squares[loc.index()];
}

void Board::force_move(Move const& move)
{
    // FIXME: Make available_moves handle the resulting `Move` changing the turn/last-pawn-move.
    auto cached_turn = flip_colour(turn);
    *this = move.result;
    turn = cached_turn;

    if (is_pawn_double_jump(move))
    {
        last_turn_pawn_double_jump_dest = move.dest;
    }
    else
    {
        last_turn_pawn_double_jump_dest = std::nullopt;
    }
}
