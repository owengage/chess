#pragma once

#include <chess/Loc.h>
#include <chess/Board.h>

namespace chess
{
    struct Move
    {
        Loc src;
        Loc dest;
        Board result;
        bool caused_check = false;
        bool is_promotion = false;
    };
}