#pragma once

#include <stdexcept>
#include <chess/square.h>

namespace chess
{
    struct Game;
    struct Move;

    struct InvalidDriverAction : std::runtime_error
    {
        using std::runtime_error::runtime_error;
    };

    struct Driver
    {
        ~Driver() = default;
        virtual Square promote(Game const&, Move const&) = 0;
        virtual void checkmate(Game const&, Move const&) = 0;
        virtual void stalemate(Game const&, Move const&) = 0;
    };
}