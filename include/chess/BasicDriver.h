#pragma once

#include <chess/Square.h>
#include <chess/Driver.h>

namespace chess
{
    struct Move;
    struct Game;

    struct BasicDriver : Driver
    {
        Square promote(Game const&, Move const&) override;
        void checkmate(Game const&, Move const&) override;
        void stalemate(Game const&, Move const&) override;
    };
}