#pragma once

#include <chess/Loc.h>
#include <chess/Move.h>

namespace chess
{
    struct Board;

    /**
     * Given a board, return all legal moves.
     */
    std::vector<Move> available_moves(Board const&);
}