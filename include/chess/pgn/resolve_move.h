#pragma once

#include <chess/Move.h>

#include <optional>

namespace chess
{
    struct Board;
}

namespace chess::pgn
{
    struct SanMove;

    /**
     * Given a move in SAN format and a current board, resolve the actual move that the SAN corresponds to.
     *
     * @return Corresponding move, or nullopt if there are none (invalid move).
     */
    std::optional<Move> resolve_move(SanMove const&, Board const&);
}