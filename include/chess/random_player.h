#pragma once

#include <chess/loc.h>
#include <chess/player.h>
#include <chess/square.h>

namespace chess
{
    struct Game;

    struct RandomPlayer : public Player
    {
        Square promote(Game const&, Loc src) override;
    };
}