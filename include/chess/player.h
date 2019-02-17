#pragma once

#include <chess/square.h>

namespace chess
{
    struct Game;

    struct Player
    {
        virtual ~Player() = default;

        /**
         * For the given game, return the piece the src square's piece will be promoted to. The piece must be exact, ie
         * the colour must be correct and be a 'royal' piece.
         */
        virtual Square promote(Game const&, Loc src) = 0;
    };
}