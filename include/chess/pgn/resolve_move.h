#pragma once

#include <optional>

namespace chess
{
    struct Move;
    struct Game;
}

namespace chess::pgn
{
    struct SanMove;

    std::optional<Move> resolve_move(SanMove const&, Game const&);
}