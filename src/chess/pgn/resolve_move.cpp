#include <chess/pgn/resolve_move.h>

#include <chess/board.h>
#include <chess/move.h>
#include <chess/available_moves.h>

using chess::Board;
using chess::Move;
using chess::pgn::SanMove;

std::optional<Move> chess::pgn::resolve_move(SanMove const& san, Board const& board)
{
    auto moves = available_moves(board);
    return std::nullopt;
}