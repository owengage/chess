#pragma once

#include <chess/loc.h>
#include <chess/board.h>

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