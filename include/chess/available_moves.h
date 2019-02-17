#pragma once

#include <chess/loc.h>
#include <chess/move.h>

namespace chess
{
    struct Game;

    std::vector<Move> available_moves(Game const &game);
}