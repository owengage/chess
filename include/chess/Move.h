#pragma once

#include <chess/Loc.h>
#include <chess/Board.h>

namespace chess
{
    enum struct MoveType
    {
        invalid,
        normal,
        check,
        checkmate,
        stalemate
    };

    struct Move
    {
        Loc src;
        Loc dest;
        Board result;
        MoveType type = MoveType::invalid;
        bool is_promotion = false;
    };
}