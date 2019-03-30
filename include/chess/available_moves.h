#pragma once

#include <chess/loc.h>
#include <chess/move.h>

namespace chess
{
    struct Board;

    /**
     * Given a board, return all legal moves.
     */
    std::vector<Move> available_moves(Board const& game);
}