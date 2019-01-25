#pragma once

#include <chess/player.h>

namespace chess
{
    struct PlayerMock : public Player
    {
        MOCK_METHOD2(promote, Square(Game const&, Loc));
    };
}